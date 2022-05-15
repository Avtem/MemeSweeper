/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include "resource.h"
#include <windows.h>
#ifdef _DEBUG
#include <av.h>
#endif // _DEBUG

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	field(gfx, 16, 16, 0.15625f),	// 0.15 is Windows 7 standard
	ai(*this, field),	// !depends on field
	sndWin(L"snd\\win.wav"),
	sndLose(L"snd\\lose.wav"),
	imgMinus(L"img/nums/-.bmp"),
	imgHotkeys(L"img/hotkeys.bmp"),
	imgWin(L"img/win.bmp"),
	imgLose(L"img/lose.bmp"),
	txtGenerate(L"img/text/Generate.bmp"),
	txtAIdidntSolve(L"img/text/AI couldn't solve.bmp"),
	txt100solvable(L"img/text/100solvable.bmp"),
	txt100unsolvable(L"img/text/100unsolvable.bmp"),
	txtRandom(L"img/text/normal.bmp"),
	txtMemesLeft(L"img/text/memesLeft.bmp"),
	txtWithout100Uns(L"img/text/without100Uns.bmp"),
	txtFromFile(L"img/text/fromFile.bmp"),
	radBtnHollow(L"img/btns/empty.bmp"),
	radBtnSelected(L"img/btns/selected.bmp"),
	radBtnSelectedGray(L"img/btns/selectedGray.bmp")
{
	field.setDrawingOffset(calcOffsetForField()); 
	field.setAI(&ai);
	Tile::gameState = &gameState;

	// load num images
	for(int i=0; i < 10; ++i)
	{
		std::wstring path = L"img/nums/" + std::to_wstring(i) + L".bmp";
		imgNums.push_back(new Image(path));
	}
}


void Game::Go()
{
	if(!ai.processing)
	{
		gfx.BeginFrame();
		
		UpdateModel();
		
		ComposeFrame();
		gfx.EndFrame();
	}
}

void Game::UpdateModel()
{
	parseKeyboard();
	parseMouse();

	if(playedSnd)
		return;

	switch (gameState)
	{
		case GameSt::GameOver:
		case GameSt::Win:
			playedSnd = true;
			field.unfoldTheField();
			
			if(gameState == GameSt::GameOver)
				sndLose.Play(1.f, 0.3f);
			if(gameState == GameSt::Win)
				sndWin.Play(1.f, 0.3f);
			
			break;
	}
}



void Game::restartGame(bool randomize)
{
	playedSnd = false;
	field.reset(true, randomize);
	sndWin.StopAll();
	sndLose.StopAll();
}

void Game::destroyObjects()
{
	// load num images
	for(int i=0; i < 10; ++i)
		delete imgNums.at(i);
}

Vei2 Game::calcOffsetForField() const
{
	Vei2 fieldSize = field.getSizeInPx();
	int scrW = Graphics::ScreenWidth;
	int scrH = Graphics::ScreenHeight;

	// avDis: offset for the field
	//return { (scrW -fieldSize.x) /2, (scrH -fieldSize.y) /2 };
	return { 30, 30 }; 
}

void Game::drawTexts()
{
	Vei2 fieldSize = field.getSizeInPx();
	Vei2 drawPos = {fieldSize.x + 70, 20};

	gfx.drawImage(drawPos.x -20, drawPos.y, txtGenerate, Colors::Magenta);
	
	gfx.drawImage(drawPos.x, drawPos.y + 30*1, txtRandom, Colors::Magenta);
	gfx.drawImage(drawPos.x, drawPos.y + 30*2, txt100solvable, Colors::Magenta);
	gfx.drawImage(drawPos.x, drawPos.y + 30*3, txtAIdidntSolve, Colors::Magenta);
	gfx.drawImage(drawPos.x, drawPos.y + 30*4, txt100unsolvable, Colors::Magenta);
	gfx.drawImage(drawPos.x, drawPos.y + 30*5, txtWithout100Uns, Colors::Black);
	gfx.drawImage(drawPos.x, drawPos.y + 30*6, txtFromFile, Colors::Black);
}

void Game::drawBtns()
{
	Vei2 fieldSize = field.getSizeInPx();
	Vei2 drawPos ={fieldSize.x + 50, 20};

	gfx.drawImage(drawPos.x, drawPos.y + 30*1, radBtnHollow);
	gfx.drawImage(drawPos.x, drawPos.y + 30*2, radBtnHollow);
	gfx.drawImage(drawPos.x, drawPos.y + 30*3, radBtnHollow);
	gfx.drawImage(drawPos.x, drawPos.y + 30*4, radBtnHollow);
	gfx.drawImage(drawPos.x, drawPos.y + 30*5, radBtnHollow);
	gfx.drawImage(drawPos.x, drawPos.y + 30*6, radBtnHollow);

	Image& selBtn = field.willBeFirstClick() ? radBtnSelected
											 : radBtnSelectedGray;
	gfx.drawImage(drawPos.x, drawPos.y + 30 *(int)field.getGenType(), selBtn);
}

void Game::drawNums()
{
	Vei2 fieldSize = field.getSizeInPx();
	Vei2 minusPos    = {fieldSize.x + 40, fieldSize.y +6};
	Vei2 drawPosDig1 = {fieldSize.x + 50, fieldSize.y +6};
	Vei2 drawPosDig2 = {fieldSize.x + 60, fieldSize.y +6};

	gfx.drawImage(drawPosDig1, txtMemesLeft);
	minusPos.x	  += 110;
	drawPosDig1.x += 108;
	drawPosDig2.x += 108;
	
	int mcount = field.getRemainingMemeCount();
	size_t digIndex = abs(mcount);
	
	if(mcount < 0)
		gfx.drawImage(mcount /10 ? minusPos : drawPosDig1, imgMinus);
	if(mcount /10)
		gfx.drawImage(drawPosDig1, *imgNums.at(digIndex /10));
	gfx.drawImage(drawPosDig2, *imgNums.at(digIndex %10));
}

void Game::drawWinning()
{
	auto offset = calcOffsetForField();
	offset += field.getSizeInPx();
	offset.x -= imgWin.getWidth();
	offset.y -= imgWin.getHeight();
	offset /= 2;
	offset += {10,10};	// just a bit to the right to make it look better

	gfx.drawImage(offset.x, offset.y, imgWin, 0x000001);
}

void Game::drawLoosing()
{
	auto offset = calcOffsetForField();
	offset += field.getSizeInPx();
	offset.x -= imgLose.getWidth();
	offset.y -= imgLose.getHeight();
	offset /= 2;
	offset += {10, 10};	// just a bit to the right to make it look better

	gfx.drawImage(offset.x, offset.y, imgLose, 0x000001);
}

void Game::parseKeyboard()
{
	while(!wnd.kbd.KeyIsEmpty())
	{
		const Keyboard::Event e = wnd.kbd.ReadKey();
		if(e.IsPress()) // ignore pressings
			continue;

		switch(e.GetCode())
		{
			case VK_UP:		field.prevGenType();	    break;
			case VK_DOWN:	field.nextGenType();	    break;
			case 'R':       restartGame();				break;
			case 'T':       restartGame(false);			break;
			case VK_ESCAPE:	PostQuitMessage(0);		    break;
		}

		ai.parseKB(e);
	}
}

void Game::parseMouse()
{
	while(!wnd.mouse.IsEmpty())
	{
		Mouse::Event ev = wnd.mouse.Read();
		if(!wnd.mouse.IsInWindow()
		   || (ev.GetType() != lmbUp && ev.GetType() != rmbUp))
			continue;

		if(gameState == GameSt::Running)
		{
			Vei2 offset = calcOffsetForField();
			field.parseMouse(ev, offset);
		}
	}
}

void Game::ComposeFrame()
{
	field.draw();

	drawTexts();
	drawBtns();
	drawNums();
	//gfx.drawImage(280, 0, imgHotkeys);	// hotkey list

	if(gameState == GameSt::Win)
		drawWinning();
	if(gameState == GameSt::GameOver)
		drawLoosing();
}

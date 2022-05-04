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
	imgHotkeys(L"img/hotkeys.bmp"),
	txtGenerate(L"img/text/Generate.bmp"),
	txtAIdidntSolve(L"img/text/AI couldn't solve.bmp"),
	txt100solvable(L"img/text/100solvable.bmp"),
	txt100unsolvable(L"img/text/100unsolvable.bmp"),
	txtRandom(L"img/text/normal.bmp"),
	txtMemesLeft(L"img/text/memesLeft.bmp"),
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
	while (!wnd.kbd.KeyIsEmpty())
	{
		const Keyboard::Event e = wnd.kbd.ReadKey();
		if(e.IsPress()) // ignore pressings
			continue;

		switch (e.GetCode())
		{
			case VK_F4:
				if(!field.willBeFirstClick())
					break;
				field.iterateGenType();
				break;
			case 'R':			
				restartGame(GetAsyncKeyState(VK_CONTROL) >= 0); 
				break;
			case VK_ESCAPE:		
				PostQuitMessage(0);		
				break;
			case VK_SPACE: 
			{
				MessageBox(wnd.getHwnd(), std::to_wstring(field.getRemainingMemeCount()).c_str(),
						   L"Memes left:", 0);
			}
			break;
		}

		ai.parseKB(e);
	}
	
	while(!wnd.mouse.IsEmpty())
	{
        Mouse::Event ev = wnd.mouse.Read();
		if( !wnd.mouse.IsInWindow() 
		|| (ev.GetType() != lmbUp && ev.GetType() != rmbUp))
			continue;
		
		if(gameState == GameSt::Running)
		{
            Vei2 offset = calcOffsetForField();
            field.parseMouse(ev, offset);
		}
	}

	if(playedSnd)
		return;

	switch (gameState)
	{
		case GameSt::GameOver:
        {
			playedSnd = true;
			sndLose.Play(1.f, 0.3f);
            //wnd.ShowMessageBox(L"You suck, man.",
            //                   L"\nHit 'R' and become better at this game.");
        }
			break;
		case GameSt::Win:
        {
			playedSnd = true;
			sndWin.Play(1.f, 0.3f);
            //wnd.ShowMessageBox(L"You won!",
            //                   L"Nice job, man.\nHit 'R' to restart.");
        }
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
	return { 20, 30 }; 
}

void Game::drawTexts()
{
	Vei2 fieldSize = field.getSizeInPx();
	Vei2 drawPos = {fieldSize.x + 60, 20};

	gfx.drawImage(drawPos.x -20, drawPos.y, txtGenerate, Colors::Magenta);
	
	gfx.drawImage(drawPos.x, drawPos.y + 30*1, txtRandom, Colors::Magenta);
	gfx.drawImage(drawPos.x, drawPos.y + 30*2, txt100solvable, Colors::Magenta);
	gfx.drawImage(drawPos.x, drawPos.y + 30*3, txtAIdidntSolve, Colors::Magenta);
	gfx.drawImage(drawPos.x, drawPos.y + 30*4, txt100unsolvable, Colors::Magenta);
}

void Game::drawBtns()
{
	Vei2 fieldSize = field.getSizeInPx();
	Vei2 drawPos ={fieldSize.x + 40, 20};

	gfx.drawImage(drawPos.x, drawPos.y + 30*1, radBtnHollow);
	gfx.drawImage(drawPos.x, drawPos.y + 30*2, radBtnHollow);
	gfx.drawImage(drawPos.x, drawPos.y + 30*3, radBtnHollow);
	gfx.drawImage(drawPos.x, drawPos.y + 30*4, radBtnHollow);

	Image& selBtn = field.willBeFirstClick() ? radBtnSelected
											 : radBtnSelectedGray;
	gfx.drawImage(drawPos.x, drawPos.y + 30 *(int)field.getGenType(), selBtn);
}

void Game::drawNums()
{
	Vei2 fieldSize = field.getSizeInPx();
	Vei2 drawPosDig1 = {fieldSize.x + 30, fieldSize.y +6};
	Vei2 drawPosDig2 = {fieldSize.x + 40, fieldSize.y +6};

	int mcount = field.getRemainingMemeCount();

	gfx.drawImage(drawPosDig1, txtMemesLeft);
	
	drawPosDig1.x += 105;
	drawPosDig2.x += 105;

	if(mcount /10)
		gfx.drawImage(drawPosDig1, *imgNums.at(mcount /10));
	gfx.drawImage(drawPosDig2, *imgNums.at(mcount %10));
}

void Game::ComposeFrame()
{
	field.draw();

	drawTexts();
	drawBtns();
	drawNums();
	//gfx.drawImage(280, 0, imgHotkeys);	// hotkey list
}

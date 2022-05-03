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
	imgHotkeys(L"img/hotkeys.bmp")
{
	field.setDrawingOffset(calcOffsetForField()); 
	Tile::gameState = &gameState;
}


void Game::Go()
{
	if(!ai.processing)
		gfx.BeginFrame();
	
	UpdateModel();

	if(!ai.processing)
	{
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
			case 'R':			restartGame(GetAsyncKeyState(VK_CONTROL) >= 0); break;
			case VK_ESCAPE:		PostQuitMessage(0);		break;
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
		
		if(gameState != GameSt::Running)
		{
			if(ev.GetType() == Mouse::Event::Type::RRelease)
				restartGame();
			
			return;
		}
		else
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
	ai.processing = true;

	gameState = GameSt::Running;
	playedSnd = false;
	field.reset(true, randomize);
	sndWin.StopAll();
	sndLose.StopAll();

	ai.processing = false;
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

void Game::ComposeFrame()
{
	field.draw();
	//gfx.drawImage(280, 0, imgHotkeys);
}

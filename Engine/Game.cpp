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
#include <av.h>

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	field(gfx, 25, 25, 0.15f),
	ai(field)	// !depends on field
{
	field.setDrawingOffset(calcOffsetForField());
	Tile::gameState = &gameState;
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	while (!wnd.kbd.KeyIsEmpty())
	{
		const Keyboard::Event e = wnd.kbd.ReadKey();
		if(e.IsPress()) // ignore pressings
			continue;

		if (e.GetCode() == 'R')
		{
			gameState = GameSt::Running;
			showedMsg = false;
			field.reset();
		}
		if(gameState != GameSt::GameOver)
		{
			ai.parseKB(e, wnd.mouse);
		}
	}
	
	while(gameState != GameSt::GameOver && !wnd.mouse.IsEmpty())
	{
		Mouse::Event ev =  wnd.mouse.Read();
		if(ev.GetType() == Mouse::Event::Type::LRelease
		|| ev.GetType() == Mouse::Event::Type::RRelease)
		{
			Vei2 offset = calcOffsetForField();
			field.parseMouse(ev, offset);
		}
	}

	if(showedMsg)
		return;

	switch (gameState)
	{
		case GameSt::GameOver:
        {
			showedMsg = true;
            wnd.ShowMessageBox(L"You suck, man.",
                               L"\nHit 'R' and become better at this game.");
        }
			break;
		case GameSt::Win:
        {
			showedMsg = true;
            wnd.ShowMessageBox(L"You won!",
                               L"Nice job, man.\nHit 'R' to restart.");
        }
			break;
	}
}

Vei2 Game::calcOffsetForField() const
{
	Vei2 fieldSize = field.getSizeInPx();
	int scrW = Graphics::ScreenWidth;
	int scrH = Graphics::ScreenHeight;

	return { (scrW -fieldSize.x) /2, (scrH -fieldSize.y) /2 };
}

void Game::ComposeFrame()
{
	field.draw();
}

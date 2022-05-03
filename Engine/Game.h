/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.h																				  *
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
#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "Field.h"
#include "AI.h"
#include "Enums.h"
#include "Sound.h"
#include "Image.h"

class Game
{
public:
	Game( class MainWindow& wnd );
	Game( const Game& ) = delete;
	Game& operator=( const Game& ) = delete;
	void Go();
	void restartGame(bool randomize = true);
private:
	void ComposeFrame();
	void UpdateModel();
	/********************************/
	/*  User Functions              */
	Vei2 calcOffsetForField() const;
	void drawTexts();
	void drawBtns();
	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */
	GameSt gameState = GameSt::Running;
	GenType generationType = GenType::random;
	Field field;
	AI ai;	// !depends on field
	bool playedSnd = false;
	Sound sndLose;
	Sound sndWin;
	Image imgHotkeys;
	Image txtGenerate;
	Image txt100unsolvable;
	Image txt100solvable;
	Image txtRandom;
	Image txtAIdidntSolve;
	Image radBtnHollow;
	Image radBtnSelected;
	/********************************/
};
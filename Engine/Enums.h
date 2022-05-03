#pragma once

#define lmbUp Mouse::Event::Type::LRelease
#define rmbUp Mouse::Event::Type::RRelease

enum class GameSt
{
	Running,
	GameOver,
	Win
};

enum GenType
{
    First = 1,
    random = 1,     // the lame usual algorithm
    solvable100,
    unsolvableForAI,
    unsolvable100,
    Last = unsolvable100
};

enum class ObjT // object type (the actual thing that resides in the tile)
{
    Meme,
    Number // the tile contains number of adj.memes
};
enum class DrawSt // drawing state
{
    Normal,
    Flag,
    // after losing:
    FatalMeme,      // click that causes lost
    CorrectFlag,    // meme that player found
    WrongFlag,      // empty cell, but player thought there's a meme in it
    HiddenMeme      // meme that player couldn't find
};
enum class ClickRes
{
    Nothing,
    GameOver,
    GameWin
};

enum class FirstClickReveal
{
    Anything,
    Num0Only,
    AnyNumber
};
#pragma once

#define lmbUp Mouse::Event::Type::LRelease
#define rmbUp Mouse::Event::Type::RRelease

enum class GameSt
{
	Running,
	GameOver,
	Win
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
    FatalMeme,
    CorrectFlag,
    WrongFlag,
    HiddenMeme
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
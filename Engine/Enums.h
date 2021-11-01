#pragma once

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
    Nothing = 0,
    GameOver,
    GameWin
};
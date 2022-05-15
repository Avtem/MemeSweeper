#pragma once

//       HERE I DESCRIBE WHAT TERMS I USE:
// Tile - smallest square.
// Area - collection of 3x3 tiles or 5x5 tiles; Usually represented by its center tile
// Field - All tiles

// Unsolvable game - game where you can't beat it using logic only
// Solvable by AI - game, that my algorithms can solve

// revealing - clicking player on some tile showing its number|fatal meme
// unfolding - showing tile's actual state after Winning|Losing by Program
// unsolved area - collection of 3x3 tiles which center tile is not satisfied
// adjacent tiles - tiles around one tile. Can be 3x3 or 5x5

//      TILE TYPES:
// Empty - tile with 0 memes around it
// Hidden - tile with meme OR number
// Black - hidden tile that user did not flag
// Flagged - hidden tile that user flagged
// Number - tile with number from 1 to 8 in it
// Revealed - tile with number (0-8) which user clicked

//      TILE TYPES (AFTER GAME IS FINISHED):
// Empty - tile with 0 memes around it
// FlaggedGood - tile with a meme which user flagged
// FlaggedBad  - tile with a number which user flagged
// Fatal - tile that caused GAME OVER state
// Number - tile with number from 1 to 8 in it
// Sneaky - tile with meme which user did not flag


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
    FIRST = 1,
    random = 1,     // the lame usual algorithm
    solvable100,
    unsolvableForAI,
    unsolvable100,
    without100Unsolved,
    loadFromFile,
    LAST = loadFromFile
};

enum class ObjT // object type (the actual thing that resides in the tile)
{
    Meme,
    Number // the tile contains number (from 0 to 8) of adj.memes
};

enum class DrawSt // drawing state
{
    Normal,
    Flagged,
    // after losing / winning:
    FatalMeme,
    FlaggedGood,
    FlaggedBad,
    Sneaky
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
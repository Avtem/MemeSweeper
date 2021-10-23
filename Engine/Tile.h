#pragma once

#include "SpriteCodex.h"
#include "Mouse.h"

enum class ObjT // object type (the actual thing that resides in the tile)
{
    Void = 0,
    Number, // the tile contains number of adj.memes
    Meme
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

class Tile
{
public:
    int numOfAdjMemes = -1; // if no number!
//////////// member functions
    bool parseMouse(Mouse::Event::Type mouseEv); // returns TRUE if you hit a meme
    void revealForLoser();
    void reset();
    ObjT Tile::getObj() const;
    void setObj(ObjT type);
    DrawSt Tile::getDrawSt() const;

private:
    ObjT obj = ObjT::Void;
    DrawSt drawState = DrawSt::Normal;
    bool revealed = false;

    static constexpr int dimension = SpriteCodex::tileSize;
};


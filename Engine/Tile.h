#pragma once

#include "SpriteCodex.h"
#include "Mouse.h"
#include "Enums.h"

class Tile
{
public:
    int numOfAdjMemes = -1; // if no number!
    Vei2 index;
//////////// member functions
    void parseMouse(Mouse::Event::Type mouseEv); // returns TRUE if you hit a meme
    void revealForLoser();
    void reset(bool resetFlag = true);
    void softReset();
    void setObj(ObjT type);
    void setNumber(int memeCount);
    void reveal();
    bool isRevealed() const;
    bool isFlagged() const;
    bool isHidden() const; // a tile that was not revealed and is not flagged
    void setFlag(bool flagged);
    DrawSt Tile::getDrawSt() const;
    ObjT Tile::getObj() const;
    
    static GameSt* gameState; // Tile class now controls game state!

private:
    ObjT obj = ObjT::Number;
    DrawSt drawState = DrawSt::Normal;
    bool revealed = false;

    static constexpr int dimension = SpriteCodex::tileSize;
};


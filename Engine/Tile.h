#pragma once

#include "Mouse.h"
#include "Enums.h"

class Tile
{
public:
    int numOfAdjMemes = -1; // meme by default.
    Vei2 index; // its index within the whole Field
//////////// member functions
    void parseMouse(Mouse::Event::Type mouseEv);
    void unfold();
    void reset(bool resetFlag = true);
    void softReset();
    void setObj(ObjT type);
    void setNumber(int memeCount);
    void reveal();
    bool isRevealed() const;
    bool isFlagged() const;
    bool isBlack() const;
    void setFlag(bool flagged);
    DrawSt Tile::getDrawSt() const;
    ObjT Tile::getObj() const;
    void hide();
    
    static GameSt* gameState; // Tile class now can control game state!

private:
    ObjT obj = ObjT::Number;
    DrawSt drawState = DrawSt::Normal;
    bool revealed = false;

    static constexpr int dimension = 32;    // dim. of 1 tile = 32 pixels
};


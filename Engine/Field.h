#pragma once

#include "Tile.h"

class Field
{
public:
    Field (Graphics& Gfx, int TilesInW, int TilesInH, float MemesFillness = 0.1);
    ~Field();
    void draw() const;
    bool parseMouse(const Mouse::Event& event, Vei2& offset); // returns true if click was fatal
    void reset();
    void setDrawingOffset(Vei2 offset);
    Vei2 getSizeInPx() const;
    
private:
    Graphics& gfx;
    int tilesInW;
    int tilesInH;
    float memesFillness;
    Tile *tiles = nullptr;
    Vei2 drawOff;

    // m.f.
    int getTilesCount() const;
    void revealEverything();
    void putMemes();
    void putNumbers();
};


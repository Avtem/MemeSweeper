#pragma once

#include "Tile.h"

class Field
{
public:
    Field (Graphics& Gfx, int TilesInW, int TilesInH, float MemesFillness = 0.1);
    ~Field();
    void draw() const;
    ClickRes parseMouse(const Mouse::Event& event, Vei2& offset); // returns true if click was fatal
    ClickRes checkWinCondition() const;
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
    const int borderThickness = 5;

    // m.f.
    int getTilesCount() const;
    void revealEverything();
    void revealAdjTiles(const Vei2& pos); // reveals 8 (if they'ren't boms) adj.tiles for tiles with 0
    void putMemes();
    bool isTileIndexBad(const Vei2& index) const;
    void putNumbers();
};


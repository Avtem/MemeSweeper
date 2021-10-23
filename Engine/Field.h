#pragma once

#include "Tile.h"

enum class GameSt
{
    Running,
    GameOver,
    Win
};

class Field
{
public:
    Field (Graphics& Gfx, int TilesInW, int TilesInH, float MemesFillness = 0.1);
    ~Field();
    void draw() const;
    void parseMouse(const Mouse& mouse) const;
    
private:
    Graphics& gfx;
    int tilesInW;
    int tilesInH;
    GameSt gameState = GameSt::Running;
    Tile *tiles = nullptr;

    // m.f.
    void generateMemes(float MemesFillness);
};


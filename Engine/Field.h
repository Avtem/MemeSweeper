#pragma once

#include "Tile.h"
#include "AI.h"
#include <random>
#include "Enums.h"
#include "Graphics.h"
#include <vector>

class Field
{
public:
    Field (Graphics& Gfx, int TilesInW, int TilesInH, float MemesFillness = 0.1);
    ~Field();
    void setAI(AI* ai_ptr);
    void draw() const;
    void parseMouse(Mouse::Event event, Vei2& offset); // returns true if click was fatal
    void parseFirstClick(Vei2 tileInd, Mouse::Event::Type eventType);
    void clickTile(Vei2 index, Mouse::Event::Type eventType);
    bool checkWinCondition() const;
    void reset(bool resetFlags = true, bool randomize = true);
    void loadFromFile(const std::wstring& path);
    void setDrawingOffset(Vei2 offset);
    Vei2 getSizeInPx() const;
    int getRand() const;
    int getMemeCount() const;
    int getRemainingMemeCount() const;
    bool willBeFirstClick() const;
    void nextGenType();
    void prevGenType();
    GenType getGenType() const;
    void unfoldTheField();

private:
    friend AI;

    std::vector<Image*> imgNumbers;	// from 1 to 8
    Image* imgHiddenTile;
    Image* imgFlagged;
    Image* imgWrongFlag;
    Image* imgCorrectMeme;
    Image* imgMine;
    Image* imgBoom;     // last player click
    Graphics& gfx;
    AI* ai = nullptr;
    GenType generationType = GenType::random;
    int tilesInW;
    int tilesInH;
    float memesFillness;
    Tile *tiles = nullptr;
    Vei2 drawOff;
    bool firstClick = true;
    const FirstClickReveal firstClickResult = FirstClickReveal::Num0Only;
    const int borderThickness = 5;
    static std::random_device randDevice;
    static std::mt19937 mt;
    static std::uniform_int_distribution<int> intDistr;

    // m.f.
    int getTileCount() const;
    void loadImages();
    void revealAdjTiles(const Vei2& pos); // reveals 8 (if they'ren't boms) adj.tiles for tiles with 0
    void putMemes();
    bool tileIsValid(const Vei2& index) const;
    void putNumbers();
    Tile& tileAt(const Vei2& index) const;
    void hideEverything();
};


#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Vei2.h"
#include "Tile.h"
#include <vector>

class AI
{
public:
    AI(class Field& inField);
    void parseKB(const Keyboard::Event& event, Mouse& mose);     // avDebug: mose for debug only

    // returns true if all mines are flagged and other tiles are revealed
    bool areaIsSolved(const Vei2& centerTile);
private:
    class Field& field;

    std::vector<Tile*> getAdjTiles(const Vei2& centerTile);
    std::vector<Tile*> getHiddenTiles(const Vei2& centerTile);
    std::vector<Tile*> getHidOverlapTiles(const Vei2& cenInd1, const Vei2& cenInd2);
    std::vector<Tile*> fullyOverlaps(const Tile* t1, const Tile* t2) const; // returns non-overlapped tiles
    void randClick() const;
    void flagObvious();
    void afterFlag();
    void traitor();
    Tile& tileAt(const Vei2& indexPos) const;
};


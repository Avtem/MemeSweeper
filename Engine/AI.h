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

private:
    class Field& field;

    // gets 8 adj.tiles (if all are valid)
    std::vector<Vei2> getAdjTilesInd(const Vei2& centerTile) const;
    // gets positions of hidden adj.cells. Returns number of found hid.tiles
    std::vector<Tile*> getHiddenTiles(const Vei2& centerTile);
    void randClick() const;
    void flagObvious();
    void afterFlag();
    Tile& tileAt(const Vei2& indexPos) const;
};


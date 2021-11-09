#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Vei2.h"
#include "Tile.h"
#include <vector>

class AI
{
public:
    friend class Field;    // avDebug only

    AI(class Field& inField);
    void parseKB(const Keyboard::Event& event, Mouse& mose);     // avDebug: mose for debug only

    // returns true if flagged count == number
    bool areaIsSolved(const Vei2& centerTile);
private:
    class Field& field;

    std::vector<Tile*> getAdjTiles(const Vei2& centerTile) const;
    std::vector<Tile*> getHiddenTiles(const Vei2& centerTile, bool includeFlagged) const;
    std::vector<Tile*> getHidOverlapTiles(const Vei2& cenInd1, const Vei2& cenInd2) const;
    std::vector<Tile*> getNonOverlapTiles(const Tile* t, const Tile* adjT) const; // returns non-overlapped tiles
    void excludeTiles(std::vector<Tile*>& mainVec, 
                                     std::vector<Tile*>& tilesToExclude) const;
    bool solvableWithoutTiles(const Tile* t, std::vector<Tile*>& tilesToExclude) const;
    bool impossibleWithoutTiles(const Tile* t, std::vector<Tile*>& overlap) const;
    int getAdjFlagCount(const Vei2& centerTile) const;
    void randClick() const;
    void flagObvious();
    void afterFlag();
    void traitor(const Vei2& centerTile);
    Tile& tileAt(const Vei2& indexPos) const;
};


#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Vei2.h"
#include "Tile.h"
#include <vector>

class AI
{
public:
    friend class Field;

    AI(class Game& game, class Field& inField);
    void parseKB(const Keyboard::Event& event);

    // returns true if flagged count == number
    bool areaIsSolved(const Vei2& centerTile) const;

    bool processing = false;    // to improve perfomance
private:
    class Field& field;
    class Game& game;

    std::vector<Tile*> getAdjTiles(const Vei2& centerTile, int outerRingCount = 1) const;
    std::vector<Tile*> getUnrevealedTiles(const Vei2& centerTile, bool includeFlagged) const;
    std::vector<Tile*> getAllHiddenTiles(bool includeFlagged) const;
    std::vector<Tile*> getHidOverlapTiles(const Vei2& cenInd1, const Vei2& cenInd2) const;
    std::vector<Tile*> getNonOverlapTiles(const Tile* t, const Tile* adjT) const; // returns non-overlapped tiles
    void excludeTiles(std::vector<Tile*>& mainVec, 
                                    const std::vector<Tile*>& tilesToExclude) const;
    bool solvableWithoutTilesOne(const Tile* t, std::vector<Tile*>& tilesToExclude) const;
    bool solvableWithTiles(const Tile* t, std::vector<Tile*>& tiles) const;
    bool impossibleWithoutTiles(const Tile* t, std::vector<Tile*>& overlap) const;
    const Tile* findUnsolvedArea(const std::vector<Tile*>& tilesToExclude) const;
    const Tile* findUnsolvedAreaWithMaxMemes(const std::vector<Tile*>& tilesToExclude) const;
    bool areaContainsTile(const Tile& t, const std::vector<Tile*> tiles) const;
    bool areaIsSolvable(const Tile& t) const;
    bool isUnsolvable(const Tile& t) const;
    int requiredCountToSolve (const Tile& t) const;
    int getAdjFlagCount(const Vei2& centerTile) const;
    void randClick() const;
    void flagObvious();
    void afterFlag();
    void traitor();
    void cantBeHere();
    void iKnowWhereTheOthers();
    void countMatters();
    void solveNeighbour();
    void useEverything();
    bool isGameUnsolvable() const;
    // creates a game, uses all algorithms, and if solved, repeats until it's unsolved
    void regenerateUntilAIcantSolve(); 
    Tile& tileAt(const Vei2& indexPos) const;
};


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

    bool areaIsSolved(const Vei2& centerTile) const;

    bool processing = false;    // to disable drawing while AI is thinking
private:
    class Field& field;
    class Game& game;

    std::vector<Tile*> getAllUnsolvedNumbers() const;
    std::vector<Tile*> getAllBlackTiles() const;
    void getAdjTiles(Tile** outputArr, const Vei2& centerTile, 
                     int outerRingCount = 1) const;
    std::vector<Tile*> getHiddenTiles(const Vei2& centerTile, bool includeFlagged) const;
    std::vector<Tile*> getHidOverlapTiles(const Vei2& cenInd1, const Vei2& cenInd2) const;
    std::vector<Tile*> getNonOverlapTiles(const Tile* t, const Tile* adjT) const; // returns non-overlapped tiles
    std::vector<Tile*> getSquareOuterRing(const Tile* t, int ringCount = 1) const;
    void excludeTiles(std::vector<Tile*>& mainVec, 
                          const std::vector<Tile*>& tilesToExclude) const;
    bool solvableWithoutTilesOne(const Tile* t, std::vector<Tile*>& tilesToExclude) const;
    bool solvableWithTiles(const Tile* t, std::vector<Tile*>& tiles) const;
    bool impossibleWithoutTiles(const Tile* t, std::vector<Tile*>& overlap) const;
    const Tile* findUnsolvedArea(const std::vector<Tile*>& tilesToExclude) const;
    const Tile* findUnsolvedAreaWithMaxMemes(const std::vector<Tile*>& tilesToExclude) const;
    bool areaContainsTile(const Tile& t, const std::vector<Tile*> tiles) const;
    bool areaIsSolvable(const Tile& t) const;
    bool surroundedWithRevealedNumber(const Tile& t) const;
    int requiredCountToSolve (const Tile& t) const;
    int getAdjFlagCount(const Vei2& centerTile) const;
    bool isAhid2x2Square(const Vei2& ind) const;
    bool isSquare2x2surroundedWithHidTiles(const Vei2& ind) const;
    bool isSquareUnsolvable(const Vei2& ind) const;

    void randClick() const;
    void flagObvious();
    void afterFlag();
    void traitor();
    void cantBeHere();
    void iKnowWhereTheOthers();
    void countMatters();
    void solveNeighbour();
    void useEverything();
    void lastSquare3();
    void iKnowWhereIsOne();

// HELPERS (shortcuts)
    Tile& tileAt(const Vei2& indexPos) const;
    // tells if pos2 is to the right of pos1
    bool toTheRight(const Vei2& pos1, const Vei2& pos2) const; 
    bool toTheLeft(const Vei2& pos1, const Vei2& pos2) const; 
    bool isAbove(const Vei2& pos1, const Vei2& pos2) const; 
    bool isBelow(const Vei2& pos1, const Vei2& pos2) const; 

    // detecting unsolvable fields
    // global stuff
    bool insideBushes() const;
    bool theSquare() const;
    bool theSquareLast() const; // 2x2 square only when there're 2 memes left
    bool thereIsSingle() const;
    bool isSingle(const Tile& t) const; // BEFORE CALLING IT, MAKE SURE you are not in WIN state

    bool isGameSolved() const;
    bool isGameUnsolvable100percent() const;
    // creates a game, uses all algorithms, and if solved, repeats until it's unsolved
    void regenerateUntilSolvable100(const Tile& tile);
    void regenerateUntilClickedTileIsSave(const Tile& tile);
    void regenerateUntilAIcantSolve(const Tile& tile);
    void regenerateUntilUnsolvable100percent(const Tile& tile);
    void regenerateWithout100Unsolv(const Tile& tile);
};


#include "AI.h"
#include "Field.h"
#include "Game.h"
#ifdef _DEBUG
#include <av.h>
#endif // _DEBUG



AI::AI(class Game& game, Field& inField)
	:field(inField),
    game(game)
{
}

bool AI::areaIsSolved(const Vei2& centerTile) const
{
    Tile& t = tileAt(centerTile);
    if(!t.isRevealed())
        return false;

    return getAdjFlagCount(centerTile) == t.numOfAdjMemes;
}


void AI::randClick() const
{
	int x = field.getRand() %field.tilesInW;
	int y = field.getRand() %field.tilesInH;

    field.clickTile({x,y}, Mouse::Event::Type::LRelease);
}

void AI::flagObvious()
{
    for(int i=0; i < field.getTileCount(); ++i)
    {
        Vei2 ind = { i %field.tilesInW, i /field.tilesInW };
        Tile& t = field.tiles[i];

        auto hidTiles = getUnrevealedTiles(ind, true);
        if(t.isRevealed() && hidTiles.size() == size_t(t.numOfAdjMemes))
        {
            for(Tile* adjT : hidTiles)
                adjT->setFlag(true);
        }
    }
}

void AI::afterFlag()
{
    for (int i = 0; i < field.getTileCount(); ++i)
    {
        Vei2 ind = { i %field.tilesInW, i /field.tilesInW };
        Tile& t = field.tiles[i];

        if (t.isRevealed() && t.numOfAdjMemes >= 0)
        {
            auto hidTiles = getUnrevealedTiles(ind, true);

            // check if the area is complete
            int flaggedCount = 0;
            for(const Tile* adjT : hidTiles)
                if(adjT->isFlagged())
                   ++flaggedCount;

            // yay, we can reveal other tiles!
            if(flaggedCount == t.numOfAdjMemes)
            {
                for(Tile* adjT : hidTiles)
                {
                    if(adjT->isHidden())
                    {
                        field.clickTile(adjT->index, Mouse::Event::Type::LRelease);
                        field.checkWinCondition();
                    }
                }
            }
        }
    }
}

void AI::traitor()
{
    afterFlag();

    for(int i=0; i < field.getTileCount(); ++i)
    {
        Tile& t = field.tiles[i];   // our main tile (that we "click")
        if(!t.isRevealed() || areaIsSolved(t.index))
            continue;

        // find potencial unsolved adjacent tiles (2 for remote traitor)
        static Tile* adjTiles[25]{};
        getAdjTiles(adjTiles, t.index, 2);

        for(int i=0; adjTiles[i]; ++i)
        {
            const Tile* adj = adjTiles[i];

            if(!adj->isRevealed() || areaIsSolved(adj->index))
                continue;

            // if t overlaps adj.cell, reveal all unrevealed tiles for t
            auto overlap = getHidOverlapTiles(t.index, adj->index);
            auto adjHidTiles = getUnrevealedTiles(adj->index, false);
            excludeTiles(adjHidTiles, overlap);
            
            // t(traitor) = solvable, adj = not
            if(solvableWithoutTilesOne(&t, overlap)
            && impossibleWithoutTiles(adj, overlap) )
            {
                auto nonOverlap = getNonOverlapTiles(&t, adj);
                for(Tile* tNonOv : nonOverlap)
                    field.clickTile(tNonOv->index, Mouse::Event::Type::LRelease);
            }
        }
    }
}

void AI::cantBeHere()
{
    for (int i = 0; i < field.getTileCount(); ++i)
    {
        Tile& t = field.tiles[i];   // our main tile (that we "click")
        if (!t.isRevealed() || areaIsSolved(t.index))
            continue;

        static Tile* tNeighbours[25]{};
        getAdjTiles(tNeighbours, t.index, 1);
        auto tHidTiles = getUnrevealedTiles(t.index, false);
        int tReqToSolve = requiredCountToSolve(t);

        for(int i=0; tNeighbours[i]; ++i)
        {
            const Tile* tNeigh = tNeighbours[i];

            if (!tNeigh->isRevealed() || requiredCountToSolve(*tNeigh) != 1)
                continue;

            // if t overlaps adj.cell, reveal all unrevealed tiles for t
            auto adjHidTiles = getUnrevealedTiles(tNeigh->index, false);
            auto overlap = adjHidTiles;
            excludeTiles(adjHidTiles, tHidTiles);
            if(adjHidTiles.size() == 0)
            {
                excludeTiles(tHidTiles, overlap);
                --tReqToSolve;
                if(tReqToSolve == 0)
                {
                    for (auto& tHid: tHidTiles)
                        field.clickTile(tHid->index, Mouse::Event::Type::LRelease);
                }
            }
        }
    }
}

void AI::iKnowWhereTheOthers()
{
    for (int i = 0; i < field.getTileCount(); ++i)
    {
        Tile& t = field.tiles[i]; // our main tile (that we "click")

        if (!t.isRevealed() || areaIsSolved(t.index))
            continue;

        // find unsolved adjacent area. 2 rings for remote iKnowWhereTheOthers
        static Tile* adjTiles[25]{};
        getAdjTiles(adjTiles, t.index, 2);
        for(int i=0; adjTiles[i]; ++i)
        {
            const Tile* adj = adjTiles[i];

            if (!adj->isRevealed() || areaIsSolved(adj->index))
                continue;

            auto overlap = getHidOverlapTiles(t.index, adj->index);
            auto tHid = getUnrevealedTiles(t.index, false);
            auto adjHid = getUnrevealedTiles(adj->index, false);
            excludeTiles(adjHid, overlap);
            
            if(areaIsSolvable(t)
            && adjHid.size()
            && requiredCountToSolve(*adj) -requiredCountToSolve(t) == adjHid.size())
            {
                for (Tile* tNonOv : adjHid)
                    tNonOv->setFlag(true); 
            }
        }
    }
}

void AI::countMatters()
{
    int memesLeft = field.getRemainingMemeCount();
    // we have 0 memes, yaY!
    if(memesLeft == 0)
    {
        for(int i = 0; i < field.getTileCount(); ++i)
            if(field.tiles[i].isHidden())
                field.clickTile(field.tiles[i].index, lmbUp);
        return;
    }

    std::vector <Tile*> overlap;
    for(int i=0, knownCount = 0; i < 100; ++i)
    {
        const Tile* t = findUnsolvedAreaWithMaxMemes(overlap);
        if (!t)
            return;

        std::vector <Tile*> newOverlap = getUnrevealedTiles(t->index, false);
        overlap.insert(overlap.begin(), newOverlap.begin(), newOverlap.end());

        knownCount += requiredCountToSolve(*t);
        if(knownCount == memesLeft) // yay, we did it!
        {
            std::vector <Tile*> safeTiles = getAllHiddenTiles(false);
            excludeTiles(safeTiles, overlap);

            for(const Tile* tile : safeTiles)
                field.clickTile(tile->index, lmbUp);
            
            return;
        }
    }
}

void AI::solveNeighbour()
{
    for (int i = 0; i < field.getTileCount(); ++i)
    {
        Tile& t = field.tiles[i]; // our main tile (that we "click")

        if (t.isHidden() || areaIsSolved(t.index)) // ignore irrelevant stuff
            continue;

        // find unsolved adjacent area. 2 rings for remote iKnowWhereTheOthers
        static Tile* adjTiles[25]{};
        getAdjTiles(adjTiles, t.index, 1);
        for(int i=0; adjTiles[i]; ++i)
        {
            const Tile* adj = adjTiles[i];

            if (!adj->isRevealed() || areaIsSolved(adj->index))
                continue;

            auto tHid = getUnrevealedTiles(t.index, false);
            auto overlap = getHidOverlapTiles(t.index, adj->index);
            excludeTiles(tHid, overlap);
            
            if(!tHid.size()
            && requiredCountToSolve(t) == requiredCountToSolve(*adj))
            {
                auto adjHid = getUnrevealedTiles(adj->index, false);
                excludeTiles(adjHid, overlap);
                for(auto& aTile : adjHid)
                    field.clickTile(aTile->index, lmbUp);
            }
        }
    }
}

void AI::useEverything()
{
    for(int i=0; i < 15; ++i)
    {
        flagObvious();
        cantBeHere();
        traitor(); // includes "afterflag"
        iKnowWhereTheOthers();
        countMatters();
        solveNeighbour();
        lastSquare3();

        if(*Tile::gameState == GameSt::Win
        || *Tile::gameState == GameSt::GameOver)
            break;
    }
}

void AI::lastSquare3()
{
    if(field.getRemainingMemeCount() != 3)
        return;

    auto hidTiles = getAllHiddenTiles(false);
    if(hidTiles.size() == 4 && isAhid2x2Square(hidTiles.at(0)->index))
    {
        auto outerRing = getSquareOuterRing(hidTiles.at(0));
        for(int i=outerRing.size() -1; i >= 0; --i)
            if(!outerRing.at(i)->isRevealed())
                outerRing.erase(outerRing.begin() +i);

        // find intersection where there's only 1 tile
        for(uint i=1; i < outerRing.size(); ++i)
        {
            auto overlap = getHidOverlapTiles(outerRing.at(0)->index,
                                    outerRing.at(i)->index);
            if(overlap.size() == 1)
            {
                field.clickTile(overlap.at(0)->index, lmbUp);
                return;
            }
        }
    }
}

bool AI::isGameUnsolvable100percent() const
{
    if(*Tile::gameState == GameSt::Win)
        return false;

    if( thereIsSingle()
    ||  theSquare()
    ||  theSquareLast()
    ||  insideBushes() )
        return true;

    //auto hid = getAllHiddenTiles(false);
    //if(hid.size() == 4
    //&& field.getRemainingMemeCount() == 1
    //&& isAhid2x2Square(hid.at(0)->index))
    //    return true;

    return false;
}

bool AI::insideBushes() const
{
    const auto hidTiles = getAllHiddenTiles(false);

    // check whether all hidden tiles aren't surounded with numbers
    for(const auto* t : hidTiles)
        if(surroundedWithRevealedNumber(*t))
            return false;

    return hidTiles.size() > (size_t)field.getRemainingMemeCount();
}

bool AI::theSquare() const
{
    const auto hidden = getAllHiddenTiles(false);

    for(const Tile* t : hidden)
    {
        if( isAhid2x2Square(t->index)
        && !isSquare2x2surroundedWithHidTiles(t->index)
        && isSquareUnsolvable(t->index))
            return true;
    }

    return false;
}

bool AI::theSquareLast() const
{
    // it's only unsolvable when there are 2 memes left
    if(field.getRemainingMemeCount() == 2)
    {
        const auto hidden = getAllHiddenTiles(false);
        return hidden.size() == 4
            && isAhid2x2Square(hidden.at(0)->index);
    }
    else
        return false;
}

bool AI::thereIsSingle() const
{
    for(int i = 0; i < field.getTileCount(); ++i)
    {
        const Tile& t = field.tiles[i];
        if(t.isRevealed() && isSingle(t))
            return true;
    }

    return false;
}

const Tile* AI::findUnsolvedArea(const std::vector<Tile*>& tilesToExclude) const
{
    // find unsolved area
    for (int i = 0; i < field.getTileCount(); ++i)
    {
        const Tile& t = field.tiles[i];
        if (t.isRevealed() && !areaIsSolved(t.index) && !areaContainsTile(t, tilesToExclude))
            return &t;
    }

    return nullptr;
}


bool compareTwo(std::pair<int,int>& i1, std::pair<int,int>& i2)
{
    return i1.second > i2.second;
}
const Tile* AI::findUnsolvedAreaWithMaxMemes(const std::vector<Tile*>& tilesToExclude) const
{
    std::vector<std::pair<int,int>> unsolvedInd;
    
    // find unsolved areas
    for (int i = 0; i < field.getTileCount(); ++i)
    {
        const Tile& t = field.tiles[i];
        if(t.isRevealed() 
        && !areaIsSolved(t.index)
        && !areaContainsTile(t, tilesToExclude))
            unsolvedInd.push_back({i, requiredCountToSolve(t)});
    }

    std::sort(unsolvedInd.begin(), unsolvedInd.end(), compareTwo);

    return unsolvedInd.size() ? &field.tiles[unsolvedInd.front().first]
                              : nullptr;
}

bool AI::areaContainsTile(const Tile& t, const std::vector<Tile*> tiles) const
{
    static Tile* adjT[25]{};
    getAdjTiles(adjT, t.index);
    
    for(int i=0; adjT[i]; ++i)
        for(const Tile* tile : tiles)
            if(adjT[i]->index == tile->index)
                return true;

    return false;
}

void AI::regenerateUntilSolvable100(const Tile& tile)
{
    while(true)
    {
        regenerateUntilClickedTileIsSave(tile);
        field.clickTile(tile.index, lmbUp);
        useEverything();

        if(isGameSolved())
            break;

        field.reset(true);
    }

    field.hideEverything();
    *Tile::gameState = GameSt::Running;
    field.clickTile(tile.index, lmbUp);
}

// IF CURRENT FIELD SATISFIES THE CLICK, IT WON'T DO ANYTHING
void AI::regenerateUntilClickedTileIsSave(const Tile& tile)
{
    int spawnAttempts = 0;
    switch(field.firstClickResult)
    {
        case FirstClickReveal::Anything:
            break;
        case FirstClickReveal::AnyNumber:
            while(tile.getObj() == ObjT::Meme)
            {
                field.reset(true);
                ++spawnAttempts;
            }
            break;
        case FirstClickReveal::Num0Only:
            while(tile.numOfAdjMemes != 0)
            {
                field.reset(true);
                ++spawnAttempts;
            }
            break;
    }

#ifdef _DEBUG
    //avPrint << L"Games generated for safe click: " << spawnAttempts 
    //    << " times.\n";
#endif // _DEBUG
}

void AI::regenerateUntilAIcantSolve(const Tile& tile)
{
    int wonCount = 0;
    while(true)
    {
        regenerateUntilClickedTileIsSave(tile);
        field.clickTile(tile.index, lmbUp);
        useEverything();

        if(*Tile::gameState == GameSt::Running
        && isGameUnsolvable100percent() == false)
            break;

        field.reset(true);
        ++wonCount;
    }

#ifdef _DEBUG
    avPrint << L"AI won game: " << wonCount << " times.\n";
#endif // _DEBUG
}

void AI::regenerateUntilUnsolvable100percent(const Tile& tile)
{
    int wonCount = 0;
    while(true)
    {
        regenerateUntilClickedTileIsSave(tile);

        field.clickTile(tile.index, lmbUp);
        useEverything();

        if(isGameUnsolvable100percent())
            break;

        field.reset(true);
        ++wonCount;
    }
}

void AI::regenerateWithout100Unsolv(const Tile& tile)
{
    while(true)
    {
        regenerateUntilClickedTileIsSave(tile);
        field.clickTile(tile.index, lmbUp);
        useEverything();

        if(isGameUnsolvable100percent() == false)
            break;

        field.reset(true);
    }

    field.hideEverything();
    *Tile::gameState = GameSt::Running;
    field.clickTile(tile.index, lmbUp);
}

Tile& AI::tileAt(const Vei2& indexPos) const
{
    return field.tiles[indexPos.x + indexPos.y *field.tilesInW];
}

bool AI::toTheRight(const Vei2& pos1, const Vei2& pos2) const
{
    return pos1.y == pos2.y && pos1.x == pos2.x -1;
}

bool AI::toTheLeft(const Vei2& pos1, const Vei2& pos2) const
{
    return pos1.y == pos2.y && pos1.x == pos2.x +1;
}

bool AI::isAbove(const Vei2& pos1, const Vei2& pos2) const
{
    return pos1.x == pos2.x && pos1.y == pos2.y +1;
}

bool AI::isBelow(const Vei2& pos1, const Vei2& pos2) const
{
    return pos1.x == pos2.x && pos1.y == pos2.y -1;
}

std::vector<Tile*> AI::getHidOverlapTiles(const Vei2& cenInd1, const Vei2& cenInd2) const
{
    std::vector<Tile*> overlap;

    auto area1 = getUnrevealedTiles(cenInd1, false);
    auto area2 = getUnrevealedTiles(cenInd2, false);

    for(Tile* t1 : area1)
    {
        for (Tile* t2 : area2)
            if(t1->index == t2->index)
                overlap.push_back(t1);
    }
    return overlap;
}

// t is the guy who has nonOverlap
std::vector<Tile*> AI::getNonOverlapTiles(const Tile* t, const Tile* adjT) const
{
    std::vector<Tile*> nonOverlap;
    auto overlap = getHidOverlapTiles(t->index, adjT->index);

    size_t requiredFlagCount = adjT->numOfAdjMemes -getAdjFlagCount(adjT->index);
    if(overlap.size() >= requiredFlagCount)
    {
        // exclude overlaps for t
        nonOverlap = getUnrevealedTiles(t->index, false);
        excludeTiles(nonOverlap, overlap);
    }

    return nonOverlap;
}

std::vector<Tile*> AI::getSquareOuterRing(const Tile* t, int ringCount) const
{
    std::vector<Tile*> tiles;

    const int sideLen = 2 +ringCount *2;
    Vei2 currPos = {t->index.x -ringCount, t->index.y -ringCount};
    for(int i=0; i < sideLen*sideLen; ++i)
    {
        if(currPos != t->index   // anything except THE 2x2 square
        && currPos != Vei2{t->index.x +1, t->index.y}
        && currPos != Vei2{t->index.x   , t->index.y +1}
        && currPos != Vei2{t->index.x +1, t->index.y +1})
        {
            if(field.tileIsValid(currPos))
                tiles.push_back(&tileAt(currPos));
        }

        // get to next row
        if(i && i %sideLen == 0)
        {
            currPos.x = t->index.x -ringCount;
            ++currPos.y;
        }
        else
            ++ currPos.x;
    }

    return tiles;
}

void AI::excludeTiles(std::vector<Tile*>& mainVec,
                                    const std::vector<Tile*>& tilesToExclude) const
{
    for(int i=(int)mainVec.size() -1; i >= 0; --i)
    {
        for(const Tile* texc : tilesToExclude)
            if(mainVec.at(i)->index == texc->index)
            {
                mainVec.erase(mainVec.begin() +i);
                break;
            }
    }
}


bool AI::solvableWithoutTilesOne(const Tile* t, std::vector<Tile*>& tilesToExclude) const
{
    auto remainingTiles = getUnrevealedTiles(t->index, false);
    excludeTiles(remainingTiles, tilesToExclude);
    
    return requiredCountToSolve(*t) == 1 
        && (int)remainingTiles.size() >= 1;
}

bool AI::solvableWithTiles(const Tile* t, std::vector<Tile*>& tiles) const
{
    return (int)tiles.size() >= requiredCountToSolve(*t);
}


bool AI::impossibleWithoutTiles(const Tile* t, std::vector<Tile*>& overlap) const
{
    auto adjTiles = getUnrevealedTiles(t->index, true);
    excludeTiles(adjTiles, overlap);
    return (int)adjTiles.size() < t->numOfAdjMemes;
}

bool AI::areaIsSolvable(const Tile& t) const
{
    auto hidTiles = getUnrevealedTiles(t.index, false);
    return int(hidTiles.size()) >= requiredCountToSolve(t);
}

bool AI::surroundedWithRevealedNumber(const Tile& t) const
{
    static Tile* tiles[25]{};
    getAdjTiles(tiles, t.index);
    
    for(int i=0; tiles[i]; ++i)
    {
        const Tile* tile = tiles[i];
        if(tile->getObj() == ObjT::Number && tile->isRevealed())
            return true;
    }
    return false;
}

bool AI::isGameSolved() const
{
    return field.getRemainingMemeCount() == 0;
}

// BEFORE CALLING IT, MAKE SURE you are not in WIN state
bool AI::isSingle(const Tile& t) const
{
    if(requiredCountToSolve(t) != 1)
        return false; // it's solvable

    auto overlap = getUnrevealedTiles(t.index, false);
    for(const Tile* hidT : overlap)
    {
        auto hidHidTiles = getUnrevealedTiles(hidT->index, false);
        for (const Tile* hidHidT : hidHidTiles)
        {
            if(hidHidT->isHidden() && !areaContainsTile(*hidHidT, overlap))
                return false; // a hidden tile? Can be solved
        }

        static Tile* hidAdj[25]{};
        getAdjTiles(hidAdj, hidT->index);

        for(int i=0; hidAdj[i]; ++i)
        {
            const Tile* hidNum = hidAdj[i];

            auto hidNumAdj = getUnrevealedTiles(hidNum->index, false);
            excludeTiles(hidNumAdj, overlap);
            if(!areaIsSolved(hidNum->index) && hidNumAdj.size() > 0)
                return false; // it is surrounded with a useful number!
        }
    }

    return true; // yyaaay, it's a single!
}

int AI::requiredCountToSolve(const Tile& t) const
{
    return t.numOfAdjMemes - getAdjFlagCount(t.index);
}

int AI::getAdjFlagCount(const Vei2& centerTile) const
{
    static Tile* adjT[25]{};
    getAdjTiles(adjT, centerTile);

    int count = 0;
    for(int i=0; adjT[i]; ++i)
    {
        const Tile* t = adjT[i];
        if(t->isFlagged())
            ++count;
    }

    return count;
}

bool AI::isAhid2x2Square(const Vei2& ind) const
{
    if(!tileAt(ind).isHidden())
        return false;

    bool tile2 = field.tileIsValid({ind.x +1, ind.y})
               && tileAt({ind.x +1, ind.y}).isHidden();
    bool tile3 = field.tileIsValid({ind.x, ind.y +1})
        && tileAt({ind.x, ind.y +1}).isHidden();
    bool tile4 = field.tileIsValid({ind.x +1, ind.y +1})
        && tileAt({ind.x +1, ind.y +1}).isHidden();

    return tile2 && tile3 && tile4;
}

bool AI::isSquare2x2surroundedWithHidTiles(const Vei2& ind) const
{
    // checks the 2 adj.tiles from each side
    const auto hidden = getSquareOuterRing(&tileAt(ind), 2);
    for(const Tile* t: hidden)
        if(t->isHidden())
            return true;

    return false;
}

  // p means Position of the topleft tile in the square
bool AI::isSquareUnsolvable(const Vei2& p) const
{
    // check if tile is valid
#define valid(X,Y) field.tileIsValid(Vei2(X,Y))
    // check if area requires 1 meme to solve
#define req1(X,Y) requiredCountToSolve(tileAt(Vei2(X,Y))) == 1

    bool topIsFilled = valid(p.x,    p.y -1) && req1(p.x,    p.y -1)
                    || valid(p.x +1, p.y -1) && req1(p.x +1, p.y -1);
    bool botIsFilled = valid(p.x,    p.y +2) && req1(p.x,    p.y +2)
                    || valid(p.x +1, p.y +2) && req1(p.x +1, p.y +2);
    if(topIsFilled && botIsFilled)
        return true;


    bool leftIsFilled  = valid(p.x -1,  p.y)    && req1(p.x -1, p.y)
                      || valid(p.x -1,  p.y +1) && req1(p.x -1, p.y +1);
    bool rightIsFilled = valid(p.x +2, p.y)    && req1(p.x +2, p.y)
                      || valid(p.x +2, p.y +1) && req1(p.x +2, p.y +1);
    return leftIsFilled && rightIsFilled;
}

void AI::parseKB(const Keyboard::Event& event)
{
    if(*Tile::gameState != GameSt::Running)
        return;
    
    processing = true;

	switch (event.GetCode())
	{
		case '1':   randClick();                  break;
		case '2':   flagObvious();                break;
		case '3':   afterFlag();                  break;
        case '4':   traitor();                    break;
        case '5':   iKnowWhereTheOthers();        break;
        case '6':   countMatters();               break;
        case '7':   cantBeHere();                 break;
        case '8':   solveNeighbour();             break;
        case '9':   lastSquare3();                break;
        case 'Q':   useEverything();              break;
        case 'Z':   isGameUnsolvable100percent(); break;
        case 'E':   randClick(); useEverything(); break; // 1-key press solving
	}
    
    processing = false;
}

void AI::getAdjTiles(Tile** outputArr, const Vei2& centerTile, 
                     int outerRingCount) const
{    
    *outputArr = nullptr;

    Vei2 adjInd{ centerTile.x -outerRingCount, centerTile.y -outerRingCount };
    int width = 1+ outerRingCount*2;
    for (int i = 0; i < width*width; ++i, ++adjInd.x)
    {
        if (i > 0 && i %width == 0)
        {
            ++adjInd.y;
            adjInd.x = centerTile.x -outerRingCount;
        }

        if (field.tileIsValid(adjInd) && centerTile != adjInd)
        {
            *outputArr = &tileAt(adjInd);
            ++outputArr;
        }
    }
    
    *outputArr = nullptr; // set end of the array
}

std::vector<Tile*> AI::getUnrevealedTiles(const Vei2& centerTile, bool includeFlagged) const
{
    static Tile* adjTiles[25]{};
    getAdjTiles(adjTiles, centerTile);

    std::vector<Tile*> hidTiles;

    for(int i=0; adjTiles[i]; ++i)
    {
        Tile* t = adjTiles[i];
        bool include = includeFlagged ? true
                                      : !t->isFlagged();
        if (!t->isRevealed() && include)
            hidTiles.push_back(t);
    }

    return hidTiles;
}

std::vector<Tile*> AI::getAllHiddenTiles(bool includeFlagged) const
{
    std::vector <Tile*> vec;
    for(int i=0; i < field.getTileCount(); ++i)
        if(!field.tiles[i].isRevealed()
        && (!includeFlagged ? !field.tiles[i].isFlagged() : true ) )
            vec.push_back(&field.tiles[i]);
            
    return vec;
}


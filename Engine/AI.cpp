#include "AI.h"
#include "Field.h"
#include "Game.h"
#include <av.h>


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
    for(int i=0; i < field.getTilesCount(); ++i)
    {
        Vei2 ind = { i %field.tilesInW, i /field.tilesInW };
        Tile& t = field.tiles[i];

        auto hidTiles = getHiddenTiles(ind, true);
        if(t.isRevealed() && hidTiles.size() == size_t(t.numOfAdjMemes))
        {
            for(Tile* adjT : hidTiles)
                adjT->setFlag(true);
        }
    }
}

void AI::afterFlag()
{
    for (int i = 0; i < field.getTilesCount(); ++i)
    {
        Vei2 ind = { i %field.tilesInW, i /field.tilesInW };
        Tile& t = field.tiles[i];

        if (t.isRevealed() && t.numOfAdjMemes >= 0)
        {
            auto hidTiles = getHiddenTiles(ind, true);

            // check if the area is complete
            int flaggedCount = 0;
            for(const Tile* adjT : hidTiles)
                if(adjT->getDrawSt() == DrawSt::Flag)
                   ++flaggedCount;

            // yay, we can reveal other tiles!
            if(flaggedCount == t.numOfAdjMemes)
            {
                for(Tile* adjT : hidTiles)
                {
                    if(!adjT->isRevealed() && adjT->getDrawSt() != DrawSt::Flag)
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

    for(int i=0; i < field.getTilesCount(); ++i)
    {
        Tile& t = field.tiles[i];   // our main tile (that we "click")
        if(!t.isRevealed() || areaIsSolved(t.index))
            continue;

        // find potencial unsolved adjacent tiles (2 for remote traitor)
        auto adjTiles = getAdjTiles(t.index, 2);
        for(const Tile* adj : adjTiles)
        {
            if(!adj->isRevealed() || areaIsSolved(adj->index))
                continue;

            // if t overlaps adj.cell, reveal all unrevealed tiles for t
            auto overlap = getHidOverlapTiles(t.index, adj->index);
            auto adjHidTiles = getHiddenTiles(adj->index, false);
            excludeTiles(adjHidTiles, overlap);
            
            // t(traitor) = solvable, adj = not
            if(solvableWithoutTiles(&t, overlap)
            && impossibleWithoutTiles(adj, overlap) )
            {
                auto nonOverlap = getNonOverlapTiles(&t, adj);
                for(Tile* tNonOv : nonOverlap)
                    field.clickTile(tNonOv->index, Mouse::Event::Type::LRelease);
            }
        }
    }
}

void AI::iKnowWhereTheOthers()
{
    for (int i = 0; i < field.getTilesCount(); ++i)
    {
        Vei2 ind = { i %field.tilesInW, i /field.tilesInW };
        Tile& t = field.tiles[i]; // our main tile (that we "click")

        if (!t.isRevealed() || areaIsSolved(ind))
            continue;

        // find unsolved adjacent area
        auto adjTiles = getAdjTiles(ind);
        for (const Tile* adj : adjTiles)
        {
            if (!adj->isRevealed() || areaIsSolved(adj->index))
                continue;

            auto overlap = getHidOverlapTiles(t.index, adj->index);
            auto tHid = getHiddenTiles(t.index, false);
            auto adjHid = getHiddenTiles(adj->index, false);
            excludeTiles(adjHid, overlap);
            
            // t is solvable (with only overlap) && adj is 4-1 == 3
            //overlap.size() == tHid.size() &&
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
        for(int i = 0; i < field.getTilesCount(); ++i)
            if(!field.tiles[i].isRevealed() 
            && field.tiles[i].getDrawSt() != DrawSt::Flag)
                field.clickTile(field.tiles[i].index, lmbUp);
        return;
    }

    std::vector <Tile*> overlap;
    for(int i=0, knownCount = 0; i < 100; ++i)
    {
        const Tile* t = findUnsolvedAreaWithMaxMemes(overlap);
        if (!t)
            return;

        std::vector <Tile*> newOverlap = getHiddenTiles(t->index, false);
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

void AI::useEverything()
{
    for(int i=0; i < 15; ++i)
    {
        flagObvious();
        traitor();
        iKnowWhereTheOthers();
        countMatters();
    }
}

const Tile* AI::findUnsolvedArea(const std::vector<Tile*>& tilesToExclude) const
{
    // find unsolved area
    for (int i = 0; i < field.getTilesCount(); ++i)
    {
        const Tile& t = field.tiles[i];
        if (t.isRevealed() && !areaIsSolved(t.index) && !areaContainsTiles(t, tilesToExclude))
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
    for (int i = 0; i < field.getTilesCount(); ++i)
    {
        const Tile& t = field.tiles[i];
        if(t.isRevealed() 
        && !areaIsSolved(t.index)
        && !areaContainsTiles(t, tilesToExclude))
            unsolvedInd.push_back({i, requiredCountToSolve(t)});
    }

    std::sort(unsolvedInd.begin(), unsolvedInd.end(), compareTwo);

    return unsolvedInd.size() ? &field.tiles[unsolvedInd.front().first]
                              : nullptr;
}

bool AI::areaContainsTiles(const Tile& t, const std::vector<Tile*> tiles) const
{
    std::vector<Tile*> adjT = getAdjTiles(t.index);
    
    for(const Tile* aTile : adjT)
        for(const Tile* tile : tiles)
            if(aTile->index == tile->index)
                return true;

    return false;
}

void AI::regenerateUntilUnsolved()
{
    int wonCount = 0;
    while(true)
    {
        game.restartGame();
        randClick(); 
        useEverything();
        if(*Tile::gameState == GameSt::Running)
            break;

        ++wonCount;
    } 
    avPrint << L"AI won game: " << wonCount << " times.\n";
}

Tile& AI::tileAt(const Vei2& indexPos) const
{
    return field.tiles[indexPos.x + indexPos.y *field.tilesInW];
}

std::vector<Tile*> AI::getHidOverlapTiles(const Vei2& cenInd1, const Vei2& cenInd2) const
{
    std::vector<Tile*> overlap;
    overlap.reserve(4); // 2 areas can have 4 tiles overlapping max

    auto area1 = getHiddenTiles(cenInd1, false);
    auto area2 = getHiddenTiles(cenInd2, false);

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
        nonOverlap = getHiddenTiles(t->index, false);
        excludeTiles(nonOverlap, overlap);
    }

    return nonOverlap;
}

void AI::excludeTiles(std::vector<Tile*>& mainVec,
                                    const std::vector<Tile*>& tilesToExclude) const
{
    for(int i=mainVec.size() -1; i >= 0; --i)
    {
        for(const Tile* texc : tilesToExclude)
            if(mainVec.at(i)->index == texc->index)
            {
                mainVec.erase(mainVec.begin() +i);
                break;
            }
    }
}


bool AI::solvableWithoutTiles(const Tile* t, std::vector<Tile*>& tilesToExclude) const
{
    auto remainingTiles = getHiddenTiles(t->index, false);
    excludeTiles(remainingTiles, tilesToExclude);
    
    return requiredCountToSolve(*t) == 1 
        && (int)remainingTiles.size() >= requiredCountToSolve(*t);
}

bool AI::impossibleWithoutTiles(const Tile* t, std::vector<Tile*>& overlap) const
{
    auto adjTiles = getHiddenTiles(t->index, true);
    excludeTiles(adjTiles, overlap);
    return (int)adjTiles.size() < t->numOfAdjMemes;
}

bool AI::areaIsSolvable(const Tile& t) const
{
    auto hidTiles = getHiddenTiles(t.index, false);
    return int(hidTiles.size()) >= requiredCountToSolve(t);
}

int AI::requiredCountToSolve(const Tile& t) const
{
    return t.numOfAdjMemes - getAdjFlagCount(t.index);
}

int AI::getAdjFlagCount(const Vei2& centerTile) const
{
    auto adjT = getAdjTiles(centerTile);
    int count = 0;
    for(const Tile* t : adjT)
        if(t->getDrawSt() == DrawSt::Flag)
            ++count;

    return count;
}

void AI::parseKB(const Keyboard::Event& event)
{
	switch (event.GetCode())
	{
		case '1':   randClick();             break;
		case '2':   flagObvious();           break;
		case '3':   afterFlag();             break;
        case '4':   traitor();               break;
        case '5':   iKnowWhereTheOthers();   break;
        case '6':   countMatters();          break;
        case 'Q':   useEverything();         break;
        case 'E':   randClick(); useEverything(); break; // 1-key press solving
        case 'U':   regenerateUntilUnsolved(); break;
	}
}

std::vector<Tile*> AI::getAdjTiles(const Vei2& centerTile, int outerRingCount) const
{
    std::vector<Tile*> vec;
    vec.reserve(8);

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
            vec.push_back(&tileAt(adjInd));
    }

    return vec;
}

std::vector<Tile*> AI::getHiddenTiles(const Vei2& centerTile, bool includeFlagged) const
{
    std::vector<Tile*> adjTiles = getAdjTiles(centerTile);

    std::vector<Tile*> hidTiles;
    hidTiles.reserve(8);

    for (Tile* t : adjTiles)
    {
        bool include = includeFlagged ? true
                                      : !(t->getDrawSt() == DrawSt::Flag);
        if (!t->isRevealed() && include)
            hidTiles.push_back(t);
    }

    return hidTiles;
}

std::vector<Tile*> AI::getAllHiddenTiles(bool includeFlagged) const
{
    std::vector <Tile*> vec;
    for(int i=0; i < field.getTilesCount(); ++i)
        if(!field.tiles[i].isRevealed()
        && (!includeFlagged ? field.tiles[i].getDrawSt() != DrawSt::Flag : true ) )
            vec.push_back(&field.tiles[i]);
            
    return vec;
}


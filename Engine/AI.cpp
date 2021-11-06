#include "AI.h"
#include "Field.h"

AI::AI(Field& inField)
	:field(inField)
{
}

bool AI::areaIsSolved(const Vei2& centerTile)
{
    if(!tileAt(centerTile).isRevealed())
        return false;

    auto adjTiles = getHiddenTiles(centerTile);
    for(auto adjT : adjTiles)
        if(adjT->getDrawSt() != DrawSt::Flag)
            return false;

    return adjTiles.size() == size_t(tileAt(centerTile).numOfAdjMemes);
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

        auto hidTiles = getHiddenTiles(ind);
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
            auto hidTiles = getHiddenTiles(ind);

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
    for(int i=0; i < field.getTilesCount(); ++i)
    {
        Vei2 ind = { i %field.tilesInW, i /field.tilesInW };
        Tile& t = field.tiles[i];

        if(!t.isRevealed() || areaIsSolved(ind))
            continue;

        // find potencial unsolved adjacent tiles
        auto adjTiles = getAdjTiles(ind);
        for(const Tile* adj : adjTiles)
        {
            if(!adj->isRevealed() || areaIsSolved(adj->index))
                continue;

            auto nonOverlap = fullyOverlaps(&t, adj);
            if(nonOverlap.size())
            {
                for(Tile* t : nonOverlap)
                    t->reveal();
            }
        }
    }
}

Tile& AI::tileAt(const Vei2& indexPos) const
{
    return field.tiles[indexPos.x + indexPos.y *field.tilesInW];
}

std::vector<Tile*> AI::getHidOverlapTiles(const Vei2& cenInd1, const Vei2& cenInd2)
{
    std::vector<Tile*> overlap;
    overlap.reserve(4); // 2 areas can have 4 tiles overlapping max

    auto area1 = getHiddenTiles(cenInd1);
    auto area2 = getHiddenTiles(cenInd2);

    for(Tile* t1 : area1)
    {
        for (Tile* t2 : area2)
            if(t1->index == t2->index)
                overlap.push_back(t1);
    }
    return overlap;
}

std::vector<Tile*> AI::fullyOverlaps(const Tile* t1, const Tile* t2) const
{
    std::vector<Tile*> nonOverlap;



    return nonOverlap;
}

void AI::parseKB(const Keyboard::Event& event, Mouse& mose)
{
    Vei2 tileInd = (mose.GetPosVei() -field.drawOff) /SpriteCodex::tileSize;
    Vei2 arr[9] = {Vei2(-1,-1)};

	switch (event.GetCode())
	{
		case '1':   randClick();    break;
		case '2':   flagObvious();  break;
		case '3':   afterFlag();    break;
	}
}

std::vector<Tile*> AI::getAdjTiles(const Vei2& centerTile)
{
    std::vector<Tile*> vec;
    vec.reserve(8);

    Vei2 adjInd{ centerTile.x -1, centerTile.y -1 };
    for (int i = 0; i < 9; ++i, ++adjInd.x)
    {
        if (i && i %3 == 0)
        {
            ++adjInd.y;
            adjInd.x = centerTile.x-1;
        }

        if (field.tileIsValid(adjInd) && centerTile != adjInd)
            vec.push_back(&tileAt(adjInd));
    }

    return vec;
}

std::vector<Tile*> AI::getHiddenTiles(const Vei2& centerTile)
{
    std::vector<Tile*> adjTiles = getAdjTiles(centerTile);

    std::vector<Tile*> hidTiles;
    hidTiles.reserve(8);

    for (Tile* t : adjTiles)
        if (t->isRevealed() == false)
            hidTiles.push_back(t);

    return hidTiles;
}


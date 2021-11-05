#include "AI.h"
#include "Field.h"

AI::AI(Field& inField)
	:field(inField)
{
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
                        adjT->parseMouse(Mouse::Event::Type::LRelease);
                        field.checkWinCondition();
                    }
                }
            }
        }
    }
}

Tile& AI::tileAt(const Vei2& indexPos) const
{
    return field.tiles[indexPos.x + indexPos.y *field.tilesInW];
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

std::vector<Vei2> AI::getAdjTilesInd(const Vei2& centerTile) const
{
    std::vector<Vei2> vec;
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
            vec.push_back(adjInd);
    }

    return vec;
}

std::vector<Tile*> AI::getHiddenTiles(const Vei2& centerTile)
{
    std::vector<Vei2> adjTiles = getAdjTilesInd(centerTile);
    
    std::vector<Tile*> hidTiles;
    hidTiles.reserve(8);
    
    for(const Vei2& index : adjTiles)
        if(tileAt(index).isRevealed() == false)
            hidTiles.push_back(&tileAt(index));

    return hidTiles;
}

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
    Vei2 arr[9];
    for(int i=0; i < field.getTilesCount(); ++i)
    {
        Tile& t = field.tiles[i];

        Vei2 ind = { i %field.tilesInW, i /field.tilesInW };
        if(t.isRevealed() && getHiddenTiles(ind, *arr) == t.numOfAdjMemes)
        {
            for(int j=0; j < t.numOfAdjMemes; ++j)
                tileAt(arr[j]).setFlag(true);
        }
    }
}

void AI::afterFlag()
{
    Vei2 arr[9];
    for (int i = 0; i < field.getTilesCount(); ++i)
    {
        Tile& t = field.tiles[i];

        Vei2 ind = { i %field.tilesInW, i /field.tilesInW };
        if (t.isRevealed() && t.numOfAdjMemes >= 0) // reveal for nums 0 as well!
        {
            const int hidCount = getHiddenTiles(ind, *arr);
            // check if the area satisfied
            int flaggedCount = 0;
            for(int j=0; j < hidCount; ++j)
                if(tileAt(arr[j]).getDrawSt() == DrawSt::Flag)
                   ++flaggedCount;

            // yay, we can reveal others!
            if(flaggedCount == t.numOfAdjMemes)
            {
                for (int j = 0; j < hidCount; ++j)
                {
                    if (tileAt(arr[j]).getDrawSt() != DrawSt::Flag)
                    {
                        tileAt(arr[j]).parseMouse(Mouse::Event::Type::LRelease);
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

std::vector<Vei2> AI::getAdjTilesInd(const Vei2& centerTile)
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

int AI::getHiddenTiles(const Vei2& index, Vei2& arr)
{
    Vei2 adjInd{ index.x -1, index.y -1 };
    int count = 0;
    for (int i = 0; i < 9; ++i, ++adjInd.x)
    {
        if (i && i %3 == 0)
        {
            ++adjInd.y;
            adjInd.x = index.x-1;
        }

        if (!field.tileIsValid(adjInd))
            continue;

        Tile& tile = field.tiles[adjInd.x +adjInd.y *field.tilesInW];
        if(index != adjInd && tile.isRevealed() == false)
        {
            (&arr)[count] = adjInd;
            ++count;
        }
    }

    return count;
}

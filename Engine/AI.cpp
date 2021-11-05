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
                field.tiles[arr[j].x +arr[j].y*field.tilesInW].setFlag(true);
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
            {
                Tile& t2 = field.tiles[arr[j].x +arr[j].y *field.tilesInW];
                if(t2.getDrawSt() == DrawSt::Flag)
                   ++flaggedCount;
            }

            // yay, we can reveal others!
            if(flaggedCount == t.numOfAdjMemes)
            {
                for (int j = 0; j < hidCount; ++j)
                {
                    Tile& t2 = field.tiles[arr[j].x +arr[j].y *field.tilesInW];
                    if (t2.getDrawSt() != DrawSt::Flag)
                    {
                        t2.parseMouse(Mouse::Event::Type::LRelease);
                        field.checkWinCondition();
                    }
                }
            }
        }
    }
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

        if (field.isTileIndexBad(adjInd))
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

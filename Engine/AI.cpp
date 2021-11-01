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

void AI::doOnlyChoice()
{
}

void AI::parseKB(const Keyboard::Event& event, Mouse& mose)
{
    Vei2 tileInd = (mose.GetPosVei() -field.drawOff) /SpriteCodex::tileSize;
    Vei2 arr[9] = {Vei2(-1,-1)};

	switch (event.GetCode())
	{
		case '1':   randClick();  break;
		case '2':   getHiddenTiles(tileInd, *arr);  break;
		case '3':   OutputDebugString(L"3\n");  break;
		case '4':   OutputDebugString(L"4\n");  break;
		default:
			OutputDebugString(L"wa was that?\n");
			break;
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

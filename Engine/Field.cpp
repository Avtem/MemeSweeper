#include "Field.h"
#include "Mouse.h"
#include <random>

#define getRand intDistr(mt)
#define lmbUp Mouse::Event::Type::LRelease
#define rmbUp Mouse::Event::Type::RRelease

std::random_device Field::randDevice;
std::mt19937 Field::mt(Field::randDevice());
std::uniform_int_distribution<int> Field::intDistr(0, 999999);

Field::Field(Graphics& Gfx, int TilesInW, int TilesInH, float MemesFillness)
    :gfx(Gfx),
    tilesInW(TilesInW),
    tilesInH(TilesInH),
    memesFillness(MemesFillness)
{
    tiles = new Tile[tilesInW * TilesInH];
    reset();
}

Field::~Field()
{
    delete [] tiles;
}

void Field::draw() const
{
    // draw borderf
    Vei2 dim = getSizeInPx();
    RectI boardR{drawOff, dim.x, dim.y};
    gfx.DrawRect(boardR.GetExpanded(borderThickness), Colors::Gray);
    // draw field
    gfx.DrawRect(boardR, SpriteCodex::baseColor);

    for(int y=0; y < tilesInH; ++y)
    {
        for(int x=0; x < tilesInW; ++x)
        {
            const Tile& tile = tiles[x +y*tilesInW];
            Vei2 tilePos{x,y};
            tilePos *= SpriteCodex::tileSize;
            tilePos += drawOff;
            
            if(!tile.isRevealed())
                SpriteCodex::DrawTileButton(tilePos, gfx);
            switch (tile.getDrawSt())
            {
                case DrawSt::HiddenMeme:   SpriteCodex::DrawTileBomb(tilePos, gfx);  break;
                case DrawSt::CorrectFlag:  SpriteCodex::DrawTileBomb(tilePos, gfx);
                                           SpriteCodex::DrawTileFlag(tilePos, gfx);  break;
                case DrawSt::Flag:         SpriteCodex::DrawTileFlag(tilePos, gfx);  break;
                case DrawSt::WrongFlag:    SpriteCodex::DrawTileCross(tilePos, gfx); break;
                case DrawSt::FatalMeme:    SpriteCodex::DrawTileBombRed(tilePos, gfx); break;
            }

            if(tile.isRevealed() && tile.getObj() == ObjT::Number)
            {
            #define drawN(n) SpriteCodex::DrawTile ## n(tilePos, gfx)
                switch (tile.numOfAdjMemes)
                {
                    case 0:  drawN(0);   break;
                    case 1:  drawN(1);   break;
                    case 2:  drawN(2);   break;
                    case 3:  drawN(3);   break;
                    case 4:  drawN(4);   break;
                    case 5:  drawN(5);   break;
                    case 6:  drawN(6);   break;
                    case 7:  drawN(7);   break;
                    case 8:  drawN(8);   break;
                }
            }
        }
    }
}

ClickRes Field::parseMouse(Mouse::Event event, Vei2& offset)
{
    Vei2 tileInd = (event.GetPosVei() -offset) /SpriteCodex::tileSize;
    
    return clickTile(tileInd, event.GetType());
}

ClickRes Field::clickTile(Vei2 index, Mouse::Event::Type eventType)
{
    if (index.x < 0 || index.y < 0 ||  index.x > tilesInW -1 || index.y > tilesInH -1)
        return ClickRes::Nothing;

    ClickRes clickRes = tiles[index.x +index.y *tilesInW].parseMouse(eventType);
    if (clickRes == ClickRes::GameOver)
    {
        revealEverything();
        return ClickRes::GameOver;
    }
    else if (eventType == lmbUp &&  0 == tiles[index.x +index.y *tilesInW].numOfAdjMemes)
    {
        revealAdjTiles(index);
    }

    return checkWinCondition();
}

ClickRes Field::checkWinCondition() const
{
    int hiddenTilesCount = 0;
    for(int i=0; i < getTilesCount(); ++i)
        hiddenTilesCount += !tiles[i].isRevealed() ? 1 : 0;

    return (int(getTilesCount() *memesFillness) == hiddenTilesCount) ? ClickRes::GameWin
                                                                     : ClickRes::Nothing;
}

int Field::getTilesCount() const
{
    return tilesInW * tilesInH;
}

void Field::revealEverything()
{
    for (int i = 0; i < getTilesCount(); ++i)
        tiles[i].revealForLoser();
}

void Field::revealAdjTiles(const Vei2& pos)
{
    // terminate condition for the recursion
    if(0 != tiles[pos.x +pos.y *tilesInW].numOfAdjMemes)
        return;

    // reveal all 9 tiles around POS
    Vei2 ind{pos.x -1, pos.y -1};
    for (int i = 0; i < 9; ++i, ++ind.x)
    {
        if (i && i %3 == 0)
        {
            ++ind.y;
            ind.x = pos.x-1;
        }

        if (isTileIndexBad(ind))
            continue;

        Tile& tile = tiles[ind.x +ind.y *tilesInW];
        if (tile.getObj() != ObjT::Meme && false == tile.isRevealed())
        {
            tile.reveal();
            revealAdjTiles(ind);    // recurse!
        }
    }
}

void Field::putMemes()
{
    int memesCount = int(getTilesCount() *memesFillness);

    for (int i = 0; i < memesCount; ++i)
    {
        int x{ 0 };
        int y{ 0 };
        do
        {
            x = getRand % tilesInW;
            y = getRand % tilesInH;
        } while (tiles[x +y*tilesInW].getObj() == ObjT::Meme);
        tiles[x +y*tilesInW].setObj(ObjT::Meme);
    }
}

bool Field::isTileIndexBad(const Vei2& index) const
{
    return index.x < 0 || index.y < 0 || index.x >= tilesInW || index.y >= tilesInH;
}

void Field::putNumbers()
{
    for (int y=0; y < tilesInH; ++y)
    {
        for(int x=0; x < tilesInW; ++x)
        {
            // don't count for memes
            if(tiles[x +y *tilesInW].getObj() == ObjT::Meme)
                continue;

            int count = 0;
            int nx = x-1;
            int ny = y-1;
            for(int i=0; i < 9; ++i, ++nx)
            {
                if(i && i %3 == 0)
                {
                    ++ny;
                    nx = x-1;
                }

                if(isTileIndexBad({nx,ny}))
                    continue;

                count += tiles[nx +ny*tilesInW].getObj() == ObjT::Meme ? 1 : 0;
            }

            tiles[x +y*tilesInW].setNumber(count);
        }
    }
}

void Field::reset()
{
    for(int i=0; i < getTilesCount(); ++i)
        tiles[i].reset();

    putMemes();
    putNumbers();
}

void Field::setDrawingOffset(Vei2 offset)
{
    drawOff = offset;
}

Vei2 Field::getSizeInPx() const
{
    return {tilesInW * SpriteCodex::tileSize, tilesInH *SpriteCodex::tileSize};
}

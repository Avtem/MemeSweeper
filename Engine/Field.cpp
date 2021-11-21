#include "Field.h"
#include "Mouse.h"
#include <random>
#include <av.h>

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

void Field::parseMouse(Mouse::Event event, Vei2& offset)
{
    Vei2 tileInd = (event.GetPosVei() -offset) /SpriteCodex::tileSize;
     
    clickTile(tileInd, event.GetType());
}

void Field::parseFirstClick(Vei2 tileInd, Mouse::Event::Type eventType)
{    
    if(!firstClick || !tileIsValid(tileInd) || eventType == rmbUp)
        return;

    int spawnAttempts = 0;
    switch (firstClickResult)
    {
        case FirstClickReveal::Anything:
            break;
        case FirstClickReveal::AnyNumber:
            while(tileAt(tileInd).getObj() == ObjT::Meme)
            {
                reset(false);
                ++spawnAttempts;
            }
            break;
        case FirstClickReveal::Num0Only:
            while (tileAt(tileInd).numOfAdjMemes != 0)
            {
                reset(false);
                ++spawnAttempts;
            }
            break;
    }

    WCHAR buff[5] = {0};
    swprintf_s(buff, L"% 3i", spawnAttempts);
    avPrint << L"We reset the field " << buff << " times, so that you can"
        << " enjoy clicking on a tile with 0 memes around it." << av::endl;
    firstClick = false;
}

void Field::clickTile(Vei2 index, Mouse::Event::Type eventType)
{
    if(!tileIsValid(index))
        return;

    if (firstClick)
        parseFirstClick(index, eventType);

    tileAt(index).parseMouse(eventType);
    if (*Tile::gameState == GameSt::GameOver)
    {
        revealEverything();
        return;
    }
    else if (eventType == lmbUp &&  0 == tiles[index.x +index.y *tilesInW].numOfAdjMemes)
    {
        revealAdjTiles(index);
    }

    checkWinCondition();
}

void Field::checkWinCondition() const
{
    int hiddenTilesCount = 0;
    for(int i=0; i < getTilesCount(); ++i)
        hiddenTilesCount += !tiles[i].isRevealed() ? 1 : 0;

    if (getMemeCount() == hiddenTilesCount)
        *Tile::gameState = GameSt::Win;
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
    if(0 != tileAt(pos).numOfAdjMemes || tileAt(pos).getDrawSt() == DrawSt::Flag)
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

        if (tileIsValid(ind) == false)
            continue;

        Tile& tile = tileAt(ind);
        if (tile.getObj() == ObjT::Number 
        && !tile.isRevealed()
        && tile.getDrawSt() != DrawSt::Flag)
        {
            tile.reveal();
            revealAdjTiles(ind);    // recurse!
        }
    }
}

void Field::putMemes()
{
    for (int i = 0; i < getMemeCount(); ++i)
    {
        int x{ 0 };
        int y{ 0 };
        do
        {
            x = getRand() % tilesInW;
            y = getRand() % tilesInH;
        } while (tiles[x +y*tilesInW].getObj() == ObjT::Meme);
        tiles[x +y*tilesInW].setObj(ObjT::Meme);
    }
}

bool Field::tileIsValid(const Vei2& index) const
{
    return index.x >= 0 && index.y >= 0 && index.x < tilesInW && index.y < tilesInH;
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

                if(tileIsValid({nx,ny}) == false)
                    continue;

                count += tiles[nx +ny*tilesInW].getObj() == ObjT::Meme ? 1 : 0;
            }

            tiles[x +y*tilesInW].setNumber(count);
        }
    }
}

Tile& Field::tileAt(const Vei2& index) const
{
    return tiles[index.x +index.y *tilesInW];
}

void Field::reset(bool resetFlags)
{
    firstClick = true;

    for(int i=0; i < getTilesCount(); ++i)
    {
        tiles[i].reset(resetFlags);
        tiles[i].index = { i %tilesInW, i /tilesInW };
    }

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

int Field::getRand() const
{
    return intDistr(mt);
}

int Field::getMemeCount() const
{
    return int(getTilesCount() *memesFillness);
}

int Field::getRemainingMemeCount() const
{
    int flaggedCount = 0;
    for (int i = 0; i < getTilesCount(); ++i)
        flaggedCount += tiles[i].getDrawSt() == DrawSt::Flag ? 1 : 0;

    return getMemeCount() -flaggedCount;
}

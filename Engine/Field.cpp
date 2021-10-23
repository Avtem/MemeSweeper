#include "Field.h"
#include "Mouse.h"

Field::Field(Graphics& Gfx, int TilesInW, int TilesInH, float MemesFillness)
    :gfx(Gfx),
    tilesInW(TilesInW),
    tilesInH(TilesInH)
{
    tiles = new Tile[tilesInW * TilesInH];

    reset(MemesFillness);
}

Field::~Field()
{
    delete [] tiles;
}

void Field::draw() const
{
    gfx.DrawRect(0, 0, gfx.ScreenWidth -1, gfx.ScreenHeight -1, SpriteCodex::baseColor);

    for(int y=0; y < tilesInH; ++y)
    {
        for(int x=0; x < tilesInW; ++x)
        {
            const Tile& tile = tiles[x +y*tilesInW];
            Vei2 tilePos{x,y};
            tilePos *= SpriteCodex::tileSize;

            if(!tile.isRevealed())
                SpriteCodex::DrawTileButton(tilePos, gfx);

            switch (tile.getDrawSt())
            {
                case DrawSt::CorrectFlag:  SpriteCodex::DrawTileFlag(tilePos, gfx);  break;
                case DrawSt::WrongFlag:    SpriteCodex::DrawTileCross(tilePos, gfx); break;
                case DrawSt::FatalMeme:    SpriteCodex::DrawTileBombRed(tilePos, gfx); break;
                case DrawSt::Flag:         SpriteCodex::DrawTileFlag(tilePos, gfx);  break;
                case DrawSt::HiddenMeme:   SpriteCodex::DrawTileBomb(tilePos, gfx);  break;
            }
        }
    }
}

void Field::parseMouse(const Mouse::Event& event) const
{
    Vei2 tileInd = event.GetPosVei() /SpriteCodex::tileSize;
    if(tileInd.x > tilesInW -1 || tileInd.y > tilesInH -1)
        return;

    tiles[tileInd.x +tileInd.y *tilesInW].parseMouse(event.GetType());
}

int Field::getTilesCount() const
{
    return tilesInW * tilesInW;
}

void Field::reset(float MemesFillness)
{
    int memesCount = int(getTilesCount() *MemesFillness);

    for(int i=0; i < getTilesCount(); ++i)
        tiles[i].reset();

    // generating memes...
    for(int i=0; i < memesCount; ++i)
    {
        int x{-1};
        int y{-1};
        do
        {
            x = rand() %tilesInW;
            y = rand() %tilesInH;
        } while (tiles[x +y*tilesInW].getObj() == ObjT::Meme);
        tiles[x +y*tilesInW].setObj(ObjT::Meme);
    }
}

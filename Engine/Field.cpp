#include "Field.h"

Field::Field(Graphics& Gfx, int TilesInW, int TilesInH, float MemesFillness)
    :gfx(Gfx),
    tilesInW(TilesInW),
    tilesInH(TilesInH)
{

}

Field::~Field()
{
    delete [] tiles;
}

void Field::draw() const
{
}

void Field::parseMouse(const Mouse& mouse) const
{
}

void Field::generateMemes(float MemesFillness)
{
}

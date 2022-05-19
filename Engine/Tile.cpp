#include "Tile.h"

#define lmbUp Mouse::Event::Type::LRelease
#define rmbUp Mouse::Event::Type::RRelease

GameSt *Tile::gameState = nullptr;

bool Tile::operator==(const Tile& other) const
{
    return index == other.index;
}

bool Tile::operator!=(const Tile& other) const
{
    return !(*this == other);
}

void Tile::parseMouse(Mouse::Event::Type mouseEv)
{
    if(revealed)
        return;

    if(mouseEv == lmbUp && drawState != DrawSt::Flagged)
        reveal();
    else if (mouseEv == rmbUp)  // toggle flag
        drawState = drawState == DrawSt::Normal ? DrawSt::Flagged
                                                : DrawSt::Normal;
}

void Tile::unfold()
{    
    if(revealed)
        return;

    if(drawState == DrawSt::Flagged)
        drawState = obj == ObjT::Meme ? DrawSt::FlaggedGood
                                      : DrawSt::FlaggedBad; // avBug
    else if(obj == ObjT::Meme)
        drawState = DrawSt::Sneaky;
}

void Tile::reset(bool resetFlag)
{
    numOfAdjMemes = -1;
    revealed = false;
    obj = ObjT::Number;

    if(resetFlag)
        drawState = DrawSt::Normal;
}

void Tile::softReset()
{
    revealed = false;
    obj = ObjT::Number;

    drawState = DrawSt::Normal;
}

ObjT Tile::getObj() const
{
    return obj;
}

void Tile::hide()
{
    revealed = false;
}

void Tile::setObj(ObjT type)
{
    obj = type;
}

void Tile::setNumber(int memeCount)
{
    obj = ObjT::Number;
    numOfAdjMemes = memeCount;
}

DrawSt Tile::getDrawSt() const
{
    return drawState;
}

bool Tile::isRevealed() const
{
    return revealed;
}

bool Tile::isFlagged() const
{
    return drawState == DrawSt::Flagged;
}

bool Tile::isBlack() const
{
    return revealed == false && isFlagged() == false;
}

void Tile::setFlag(bool flagged)
{
    drawState = flagged ? DrawSt::Flagged : DrawSt::Normal;
}


void Tile::reveal()
{
    revealed = true;
    
    if(obj == ObjT::Meme)
    {
        drawState = DrawSt::FatalMeme;
        *gameState = GameSt::GameOver;
    }
}
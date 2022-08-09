#include "Tile.h"

#define lmbUp Mouse::Event::Type::LRelease
#define rmbUp Mouse::Event::Type::RRelease

GameSt* Tile::gameState = nullptr;

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
        setFlag(!isFlagged());
}

void Tile::unfold()
{    
    if(revealed)
        return;

    if(isFlagged())
        drawState = getObj() == ObjT::Meme ? DrawSt::FlaggedGood
                                           : DrawSt::FlaggedBad;
    else if(getObj() == ObjT::Meme)
        drawState = DrawSt::Sneaky;
}

void Tile::reset(bool resetFlag)
{
    numOfAdjMemes = (int)ObjT::Unitialized;
    revealed = false;

    if(resetFlag)
        drawState = DrawSt::Normal;
}

void Tile::softReset()
{
    revealed = false;

    drawState = DrawSt::Normal;
}

ObjT Tile::getObj() const
{
    if(numOfAdjMemes == -1)
        return ObjT::Meme;
    else
        return ObjT::Number;
}

void Tile::setMeme()
{
    numOfAdjMemes = -1;
}

void Tile::hide()
{
    revealed = false;
}

void Tile::setNumber(int memeCount)
{
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
    
    if(getObj() == ObjT::Meme)
    {
        drawState = DrawSt::FatalMeme;
        *gameState = GameSt::GameOver;
    }
}
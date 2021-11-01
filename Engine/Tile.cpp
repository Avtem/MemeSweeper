#include "Tile.h"

#define lmbUp Mouse::Event::Type::LRelease
#define rmbUp Mouse::Event::Type::RRelease

GameSt *Tile::gameState = nullptr;

void Tile::parseMouse(Mouse::Event::Type mouseEv)
{
    if(revealed)
        return;

    if(mouseEv == lmbUp && drawState != DrawSt::Flag)
    {
        switch (obj)
        {
            case ObjT::Meme:
                drawState = DrawSt::FatalMeme;
                revealed = true;
                *gameState = GameSt::GameOver;
            case ObjT::Number:
                revealed = true;
                break;
        }
    }
    else if (mouseEv == rmbUp)
    {
        // toggle flag
        drawState = drawState == DrawSt::Normal ? DrawSt::Flag : DrawSt::Normal;
    }
}

void Tile::revealForLoser()
{
    if(revealed)
        return;
    
    if(drawState == DrawSt::Flag)
        drawState = obj == ObjT::Meme ? DrawSt::CorrectFlag
                                      : DrawSt::WrongFlag;
    else if(obj == ObjT::Meme)
        drawState = DrawSt::HiddenMeme;
}

void Tile::reset()
{
    numOfAdjMemes = -1;
    revealed = false;
    obj = ObjT::Number;
    drawState = DrawSt::Normal;
}

ObjT Tile::getObj() const
{
    return obj;
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

void Tile::setFlag(bool flagged)
{
    drawState = flagged ? DrawSt::Flag : DrawSt::Normal;
}

void Tile::reveal()
{
    revealed = true;
}
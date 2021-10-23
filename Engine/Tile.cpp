#include "Tile.h"

#define lmbUp Mouse::Event::Type::LRelease
#define rmbUp Mouse::Event::Type::RRelease

bool Tile::parseMouse(Mouse::Event::Type mouseEv)
{
    if(revealed)
        return false;

    if(mouseEv == lmbUp)
    {
        switch (obj)
        {
            case ObjT::Meme:
                drawState = DrawSt::FatalMeme;
                revealed = true;
                return true;
            case ObjT::Void:
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

    return false;
}

void Tile::revealForLoser()
{
    if(false == revealed && drawState == DrawSt::Flag)
        drawState = obj == ObjT::Meme ? DrawSt::CorrectFlag : DrawSt::WrongFlag;
}

void Tile::reset()
{
    numOfAdjMemes = -1;
    revealed = false;
    obj = ObjT::Void;
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

DrawSt Tile::getDrawSt() const
{
    return drawState;
}

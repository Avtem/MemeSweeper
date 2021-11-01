#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Vei2.h"


class AI
{
public:
    AI(class Field& inField);
    void parseKB(const Keyboard::Event& event, Mouse& mose);     // avDebug: mose for debug only

private:
    class Field& field;

    // gets positions of hidden adj.cells. Returns number of found hid.tiles
    int getHiddenTiles(const Vei2& index, Vei2& arr);
    void randClick() const;
    void flagObvious();
};


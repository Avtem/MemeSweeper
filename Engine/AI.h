#pragma once

#include "Keyboard.h"

class AI
{
public:
    AI(class Field& inField);
    void randClick() const;
    void parseKB(const Keyboard::Event& event);

private:
    class Field& field;
};


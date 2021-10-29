#pragma once

#include "Keyboard.h"

class AI
{
public:
    void randClick(class Field& field) const;
    void parseKB(const Keyboard::Event& event);
};


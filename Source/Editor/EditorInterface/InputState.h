#pragma once
#include "CommonUtilities/Vector2.hpp"

struct InputState
{
    CU::Vector2I MouseDelta;
    CU::Vector2U MousePos;
    bool MousePressed;
    bool MouseHeld;
    bool MouseReleased;
    bool SHIFT;
    bool CTRL;
};


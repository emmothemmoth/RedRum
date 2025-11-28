#pragma once
#include "../../Utilities/CommonUtilities/Vector.hpp"

struct DebugBuffer
{
    float Exposure;
    int Tonemap;
    int BloomMode;
    int LuminanceMode;
    int SSAOActive;
    CU::Vector3f Padding;
};
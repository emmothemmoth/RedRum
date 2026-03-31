#pragma once
#include "CommonUtilities/Vector3.hpp"
#include <vector>

struct RayBounce
{
    CU::Vector3f StartPos;
    CU::Vector3f EndPos;
    float StartPower;
    float EndPower;
};

struct VisualRayPath
{
    std::vector<RayBounce> Bounces;
    bool HitListener = false;
};
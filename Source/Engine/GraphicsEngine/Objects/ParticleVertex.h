#pragma once

#include "CommonUtilities\Vector.hpp"
#include <vector>
struct ParticleVertex
{

    CU::Vector4f Position;
    CU::Vector4f Color;
    CU::Vector3f Velocity;
    float LifeTime;
    float Angle;
    CU::Vector2f Size;
    CU::Vector4f ChannelMask;

    static const std::vector<VertexElementDesc> InputLayoutDefinition;
};
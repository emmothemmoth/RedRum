#pragma once

#include "../../Utilities/CommonUtilities/Vector.hpp"
#include <vector>
#include "../InterOp/RHIStructs.h"


struct SpriteVertex
{
    SpriteVertex(float aX, float aY, float aU, float aV)
    {
        position =  CU::Vector4f(aX, aY, 1, 1);
        tint =  CU::Vector4f(1, 1, 1, 1);
        uv =  CU::Vector2f(aU, aV);
    }

    CU::Vector4f position;
    CU::Vector4f tint;
    CU::Vector2f uv;

    static const std::vector<VertexElementDesc> InputLayoutDefinition;
};
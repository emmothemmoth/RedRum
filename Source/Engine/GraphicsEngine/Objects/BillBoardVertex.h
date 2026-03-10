#pragma once
#include "CommonUtilities/Vector3.hpp"

#include <vector>

typedef std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementList;

struct BillboardVertex
{
    BillboardVertex(const CU::Vector3f& aCenterPosition, const CU::Vector2f& aCorner, const CU::Vector2f& aUV)
    {
        CenterPosition = aCenterPosition;
        Corner = aCorner;
        UV = aUV;
    }
    CU::Vector3f CenterPosition;
    CU::Vector2f Corner; // (-1,-1) (1,-1) (1,1) (-1,1)
    CU::Vector2f UV;

    static const std::vector<VertexElementDesc> InputLayoutDefinition;
};

const CU::Vector3f BillboardCenter = { 0,0,0 };
const std::vector<BillboardVertex> BillboardVertices =
{
    // Center,     Corner (Scale),    UV (Sampling)
    { {0,0,0},    {-25.f, -25.f},    {0.f, 1.f} }, // BL
    { {0,0,0},    { 25.f, -25.f},    {1.f, 1.f} }, // BR
    { {0,0,0},    {-25.f,  25.f},    {0.f, 0.f} }, // TL
    { {0,0,0},    { 25.f,  25.f},    {1.f, 0.f} }  // TR
};
const std::vector<unsigned> BillboardIndices = {
    0, 2, 1, // Triangle 1 (BL -> TL -> BR)
    1, 2, 3  // Triangle 2 (BR -> TL -> TR)
};

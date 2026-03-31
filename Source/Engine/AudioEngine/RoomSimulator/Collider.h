#pragma once

#include <variant>

#include "CommonUtilities/Vector3.hpp"

struct AABBCollider
{
    CU::Vector3f MinPoint;
    CU::Vector3f MaxPoint;
};

struct MeshCollider
{
    // std::vector<CU::Vector3f> Vertices; // For later!
};

struct Collider
{
    std::variant<AABBCollider, MeshCollider> Shape;
};

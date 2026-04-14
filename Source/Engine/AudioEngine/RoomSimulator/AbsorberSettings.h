#pragma once
#include <cstdint>

#include "CommonUtilities/Vector3.hpp"

struct AbsorberSettings
{

	CU::Vector3f Reflection = { 0.95f, 0.95f, 0.95f };
	float ScatteringCoefficient = 0.05f;

	uint32_t MaterialTypeID = 0;
};
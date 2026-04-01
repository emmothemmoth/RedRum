#pragma once
#include <cstdint>

struct AbsorberSettings
{

	float ReflectionCoefficient = 0.95f;
	float ScatteringCoefficient = 0.05f;

	uint32_t MaterialTypeID = 0;
};
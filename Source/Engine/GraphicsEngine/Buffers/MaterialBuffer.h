#pragma once

#include "CommonUtilities\Vector.hpp"


struct MaterialBufferData
{
	CU::Vector4f AlbedoColor;
	CU::Vector2f UVTiling;
	float NormalStrength; 
	float Shininess;
	float EmissionStrength;
	CU::Vector3f Padding;
};

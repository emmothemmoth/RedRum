#pragma once
#include "CommonUtilities\Vector.hpp"
#include "CommonUtilities\Matrix4x4.hpp"

#include <array>


struct LightBuffer
{
	struct DirLightData
	{
		CU::Vector3f Color;
		float Intensity;
		CU::Vector4f LightPos;
		CU::Vector4f LightDir;
		CU::Matrix4x4f LightViewInv;
		CU::Matrix4x4f LightProj;
		int Active = 0;
		CU::Vector3f Padding;
	} DirLight;
	struct PointLightData
	{
		CU::Vector3f Color;
		float Intensity;
		CU::Vector3f LightPos;
		int CastShadows;
		CU::Matrix4x4f LightViewInv;
		CU::Matrix4x4f LightProj;
		int Active = 0;
		float Range;
		CU::Vector2f Padding;
	} PointLights[4];

	struct SpotLightData
	{
		CU::Vector3f Color;
		float Intensity;
		CU::Vector3f LightPos;
		int CastShadows;
		CU::Vector3f LightDir;
		float InnerConeAngle;
		CU::Matrix4x4f LightViewInv;
		CU::Matrix4x4f LightProj;
		int Active = 0;
		float Range;
		float OuterConeAngle;
		float Padding;
	}SpotLights[4];

	int PointLightCount;
	int SpotLightCount;

	CU::Vector2f padding;
};
#pragma once
#include "CommonUtilities\Matrix.hpp"

struct ObjectBufferData
{
	CommonUtilities::Matrix4x4<float> Transform;
	CommonUtilities::Matrix4x4<float> InverseTranspose;
	CommonUtilities::Matrix4x4<float> BoneTransforms[128]{};
	unsigned HasBone = false;
	unsigned IsInstanced = false;
	CommonUtilities::Vector2<float> Padding;
};

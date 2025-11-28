#pragma once
#include "../Utilities/CommonUtilities/Matrix4x4.hpp"
#include <string>

struct Socket
{
	CommonUtilities::Matrix4x4<float> restTransform;
	int parentBoneIndex = -1;
	std::string name;
};

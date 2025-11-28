#pragma once
#include "../../Utilities/CommonUtilities/Matrix4x4.hpp"


struct FrameBufferData
{
	CommonUtilities::Matrix4x4<float> View;
	CommonUtilities::Matrix4x4<float> Projection;
	CommonUtilities::Vector4<float> CameraPosition;
	CommonUtilities::Vector2<float> Resolution;
	CommonUtilities::Vector2<float> Padding;
};

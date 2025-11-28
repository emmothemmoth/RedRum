#pragma once

#include "CommonUtilities\Vector.hpp"

constexpr unsigned kernelSize = 64;
struct PostProcessBuffer
{

	struct PPBufferData
	{
		CU::Vector4f Kernel[64];
		int KernelSize = kernelSize;
		CU::Vector3f Padding;
	}PostProcessData;
};


#pragma once

#include "CommonUtilities\Matrix4x4.hpp"

#include "d3d11_1.h"
#include <wrl/client.h>

#include <vector>

class InstanceData
{
public:
	InstanceData() = default;
	~InstanceData() = default;
	void AddTransform(const CU::Matrix4x4f& aTransform);
	bool Init();
	const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetInstanceBuffer() const;
	const std::vector<CU::Matrix4x4<float>>& GetRelativeTransforms() const;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> myInstanceBuffer;
	std::vector<CU::Matrix4x4<float>> myRelativeTransforms;

};

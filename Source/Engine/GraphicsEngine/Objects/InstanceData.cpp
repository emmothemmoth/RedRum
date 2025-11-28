#include "GraphicsEngine.pch.h"
#include "InstanceData.h"

#include "GraphicsEngine.h"

void InstanceData::AddTransform(const CU::Matrix4x4f& aTransform)
{
	myRelativeTransforms.emplace_back(aTransform);
}

bool InstanceData::Init()
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer;
	if (!GraphicsEngine::GetRHI()->CreateVertexBuffer("Mesh Instance Buffer", myRelativeTransforms, instanceBuffer, true))
	{
		return false;
	}
	myInstanceBuffer = std::move(instanceBuffer);

    return true;
}

const Microsoft::WRL::ComPtr<ID3D11Buffer>& InstanceData::GetInstanceBuffer() const
{
	return myInstanceBuffer;
}

const std::vector<CU::Matrix4x4<float>>& InstanceData::GetRelativeTransforms() const
{
	return myRelativeTransforms;
}

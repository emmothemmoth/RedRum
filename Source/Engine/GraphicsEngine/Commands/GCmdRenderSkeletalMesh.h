#pragma once

#include <memory>
#include<vector>

#include "GraphicsCommandList.h"
#include "..\Objects/MeshAsset.h"
#include "../Utilities/CommonUtilities\Matrix.hpp"

class GCmdRenderSkeletalMesh : public GraphicsCommandBase
{
public:
	GCmdRenderSkeletalMesh(const std::shared_ptr<MeshAsset> aMesh, CU::Matrix4x4f aTransform, const std::vector<CommonUtilities::Matrix4x4<float>> aBoneTransformList, std::vector<std::shared_ptr<MaterialAsset>> aMaterialList);

	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<MeshAsset> myMesh;
	CommonUtilities::Matrix4x4<float> myTransform;
	std::vector<CommonUtilities::Matrix4x4<float>> myBoneTransforms;
	std::vector<std::shared_ptr<MaterialAsset>> myMaterialList;
};

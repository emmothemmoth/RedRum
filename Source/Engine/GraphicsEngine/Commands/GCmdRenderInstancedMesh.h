#pragma once

#include <memory>
#include<vector>

#include "GraphicsCommandList.h"
#include "..\Objects\MeshAsset.h"
#include "CommonUtilities\Matrix.hpp"


class MeshAsset;
class MaterialAsset;
class InstanceData;
class GCmdRenderInstancedMesh : public GraphicsCommandBase
{
public:
	GCmdRenderInstancedMesh(const std::shared_ptr<MeshAsset> aMesh, const CU::Matrix4x4f& anObjectTransform, const std::shared_ptr<InstanceData> someInstanceData, std::vector<std::shared_ptr<MaterialAsset>> aMaterialList);

	void Execute() override;
	void Destroy() override;

private:
	std::shared_ptr<MeshAsset> myMesh;
	std::shared_ptr<InstanceData> myInstanceData;
	std::vector<std::shared_ptr<MaterialAsset>> myMaterialList;
	CU::Matrix4x4f myTransform;
};

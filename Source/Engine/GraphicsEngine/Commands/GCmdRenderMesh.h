#pragma once
#include <memory>
#include<vector>

#include "GraphicsCommandList.h"
#include "../Utilities/CommonUtilities\Matrix.hpp"

class MeshAsset;
class MaterialAsset;
class GCmdRenderMesh : public GraphicsCommandBase
{
public:
	GCmdRenderMesh(std::shared_ptr<MeshAsset> aMesh, CU::Matrix4x4f aTransform, std::vector<std::shared_ptr<MaterialAsset>> aMaterialList);

	void Execute() override;
	void Destroy() override;

private:
	std::shared_ptr<MeshAsset> myMesh;
	CommonUtilities::Matrix4x4<float> myTransform;
	std::vector<std::shared_ptr<MaterialAsset>> myMaterialList;

};

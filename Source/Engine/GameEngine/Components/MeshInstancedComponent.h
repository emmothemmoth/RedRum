#pragma once

#include "Component.h"
#include <vector>

class MeshAsset;
class MaterialAsset;
class InstanceData;

class MeshInstancedComponent : public Component
{
public:
	MeshInstancedComponent() = delete;
	MeshInstancedComponent(GameObject& aParent, std::shared_ptr<MeshAsset> aMesh, bool aShouldRenderLines = true);
	~MeshInstancedComponent();
	void Update(const float aDeltaTime) override;

	void Render() override;

	void AddInstance(const CU::Matrix4x4f& anInstanceTransform);

	void Init();

	void AddMaterial(std::shared_ptr<MaterialAsset> aMaterial);

	std::shared_ptr<MeshAsset> GetMesh() const;
	const std::vector <std::shared_ptr<MaterialAsset>>& GetMaterials() const { return myMaterials; }
	std::shared_ptr<InstanceData> GetInstanceData() const;

private:
	std::shared_ptr<MeshAsset> myMesh;
	std::vector <std::shared_ptr<MaterialAsset>> myMaterials;
	std::shared_ptr<InstanceData> myInstanceData;
};

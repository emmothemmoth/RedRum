#pragma once

#include "Component.h"
#include <vector>

class MeshAsset;
class MaterialAsset;

class MeshComponent : public Component
{
public:
	MeshComponent() = delete;
	MeshComponent(GameObject& aParent, std::shared_ptr<MeshAsset> aMesh, bool aShouldRenderLines = true);
	~MeshComponent();
	void Update(const float aDeltaTime) override;

	void Render() override;

	std::shared_ptr<MeshAsset> GetMesh();

	void AddMaterial(std::shared_ptr<MaterialAsset> aMaterial);
	const std::vector <std::shared_ptr<MaterialAsset>>& GetMaterials() const { return myMaterials; }

private:
	std::shared_ptr<MeshAsset> myMesh;
	std::vector<std::shared_ptr<MaterialAsset>> myMaterials;
};

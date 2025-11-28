#pragma once

#include "Component.h"

class MeshAsset;

class MeshComponent : public Component
{
public:
	MeshComponent() = delete;
	MeshComponent(GameObject& aParent, std::shared_ptr<MeshAsset> aMesh);
	~MeshComponent();
	void Update(const float aDeltaTime) override;

	void Render() override;

	std::shared_ptr<MeshAsset> GetMesh();

private:
	std::shared_ptr<MeshAsset> myMesh;
};

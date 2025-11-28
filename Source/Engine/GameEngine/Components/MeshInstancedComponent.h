#pragma once


#include "Component.h"

class MeshAsset;
class InstanceData;

class MeshInstancedComponent : public Component
{
public:
	MeshInstancedComponent() = delete;
	MeshInstancedComponent(GameObject& aParent, std::shared_ptr<MeshAsset> aMesh);
	~MeshInstancedComponent();
	void Update(const float aDeltaTime) override;

	void Render() override;

	void AddInstance(const CU::Matrix4x4f& anInstanceTransform);

	void Init();

	std::shared_ptr<MeshAsset> GetMesh() const;
	std::shared_ptr<InstanceData> GetInstanceData() const;

private:
	std::shared_ptr<MeshAsset> myMesh;
	std::shared_ptr<InstanceData> myInstanceData;
};

#include "GameEngine.pch.h"
#include "MeshInstancedComponent.h"
#include "AnimationComponent.h"
#include "MaterialComponent.h"
#include "GameObject.h"
#include "MainSingleton.h"

#include "../GraphicsEngine/Objects/MeshAsset.h"
#include "../GraphicsEngine/Objects/InstanceData.h"
#include "../GraphicsEngine/GraphicsEngine.h"
#include "../GraphicsEngine/Commands/GCmdRenderInstancedMesh.h"

MeshInstancedComponent::MeshInstancedComponent(GameObject& aParent, std::shared_ptr<MeshAsset> aMesh)
	: Component(aParent)
{
	myMesh = aMesh;
	myInstanceData = std::make_shared<InstanceData>();
}

MeshInstancedComponent::~MeshInstancedComponent()
{
}

void MeshInstancedComponent::Update(const float aDeltaTime)
{
	UNREFERENCED_PARAMETER(aDeltaTime);
}

void MeshInstancedComponent::Render()
{
	std::vector<std::shared_ptr<MaterialAsset>> materialList;
	if (this->GetParent().GetComponent<MaterialComponent>())
	{
		materialList = this->GetParent().GetComponent<MaterialComponent>()->GetMaterials();
	}
	else
	{
		materialList = GraphicsEngine::Get().GetDefaultMaterials();
	}
	//TODO: Check if animated, then use a different instancerendercommand OR change current to take in a bonetransformlist
	MainSingleton::Get().GetRenderer().Enqueue<GCmdRenderInstancedMesh>(myMesh, myParent.GetTransform(), myInstanceData,  materialList);
}

void MeshInstancedComponent::AddInstance(const CU::Matrix4x4f& anInstanceTransform)
{
	assert(myInstanceData);
	myInstanceData->AddTransform(anInstanceTransform);
}

void MeshInstancedComponent::Init()
{
	myInstanceData->Init();
}

std::shared_ptr<MeshAsset> MeshInstancedComponent::GetMesh() const
{
	assert(myMesh);
	return myMesh;
}

std::shared_ptr<InstanceData> MeshInstancedComponent::GetInstanceData() const
{
	assert(myInstanceData);
	return myInstanceData;
}

#include "GameEngine.pch.h"
#include "MeshInstancedComponent.h"
#include "AnimationComponent.h"
#include "GameObject.h"
#include "MainSingleton.h"

#include "../GraphicsEngine/Objects/MeshAsset.h"
#include "../../GraphicsEngine/Objects/MaterialAsset.h"
#include "../GraphicsEngine/Objects/InstanceData.h"
#include "../GraphicsEngine/GraphicsEngine.h"
#include "../GraphicsEngine/Commands/GCmdRenderInstancedMesh.h"

MeshInstancedComponent::MeshInstancedComponent(GameObject& aParent, std::shared_ptr<MeshAsset> aMesh)
	: Component(aParent)
{
	myComponentType = ComponentType::MeshInstance;
	myMesh = aMesh;
	myInstanceData = std::make_shared<InstanceData>();
	myRenderStages.at(RenderStage::ShadowMapping) = true;
	myRenderStages.at(RenderStage::Deferred) = true;
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
	if (myMaterials.empty())
	{
		myMaterials = GraphicsEngine::Get().GetDefaultMaterials();
	}
	for (auto& [renderStage, shouldRender] : myRenderStages)
	{
		if (shouldRender)
		{
			MainSingleton::Get().GetRenderer().Enqueue<GCmdRenderInstancedMesh>(renderStage, myMesh, myParent.GetTransform(), myInstanceData, myMaterials);
		}
	}
	//TODO: Check if animated, then use a different instancerendercommand OR change current to take in a bonetransformlist
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

void MeshInstancedComponent::AddMaterial(std::shared_ptr<MaterialAsset> aMaterial)
{
	myMaterials.push_back(aMaterial);
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

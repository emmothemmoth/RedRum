#include "GameEngine.pch.h"
#include "MeshComponent.h"
#include "AnimationComponent.h"
#include "MainSingleton.h"

#include "../../AssetManager/AssetManager.h"
#include "GameObject.h"
#include "../../GraphicsEngine/Objects/MeshAsset.h"
#include "../../GraphicsEngine/Objects/MaterialAsset.h"

#include "..\GraphicsEngine\Commands\GCmdRenderMesh.h"
#include "..\GraphicsEngine\Commands\GCmdRenderSkeletalMesh.h"

#include "..\GraphicsEngine\GraphicsEngine.h"

MeshComponent::MeshComponent(GameObject& aParent, std::shared_ptr<MeshAsset> aMesh)
	: Component(aParent)
{
	myMesh = aMesh;
	myComponentType = ComponentType::Mesh;
	myRenderStages.at(RenderStage::ShadowMapping) = true;
	myRenderStages.at(RenderStage::Deferred) = true;
}

MeshComponent::~MeshComponent()
{
}

void MeshComponent::Update(const float aDeltaTime)
{
	UNREFERENCED_PARAMETER(aDeltaTime);
}

void MeshComponent::Render()
{
	if (!myIsVisible) return;
	if (myMaterials.empty())
	{
		myMaterials = GraphicsEngine::Get().GetDefaultMaterials();
	}
	for (auto& [renderStage, shouldRender] : myRenderStages)
	{
		if (shouldRender)
		{
			if(this->GetParent().GetComponent<AnimationComponent>().get() != nullptr)
			{
				MainSingleton::Get().GetRenderer().Enqueue<GCmdRenderSkeletalMesh>(renderStage, myMesh, myParent.GetTransform(),
					this->GetParent().GetComponent<AnimationComponent>()->GetBoneTransforms(), myMaterials);
			}
		else
		{
			MainSingleton::Get().GetRenderer().Enqueue<GCmdRenderMesh>(renderStage, myMesh, myParent.GetTransform(), myMaterials);
		}
		}
	}
	//if (this->GetParent().GetComponent<AnimationComponent>().get() != nullptr)
	//{
	//	MainSingleton::Get().GetRenderer().Enqueue<GCmdRenderSkeletalMesh>(myMesh, myParent.GetTransform(),
	//		this->GetParent().GetComponent<AnimationComponent>()->GetBoneTransforms(),  myMaterials);
	//}
	//else
	//{
	//	MainSingleton::Get().GetRenderer().Enqueue<GCmdRenderMesh>(myMesh, myParent.GetTransform(), myMaterials);
	//}
}

std::shared_ptr<MeshAsset> MeshComponent::GetMesh()
{
	return myMesh;
}

void MeshComponent::AddMaterial(std::shared_ptr<MaterialAsset> aMaterial)
{
	myMaterials.push_back(aMaterial);
}

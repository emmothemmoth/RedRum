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
#include "BoxComponent.h"

MeshComponent::MeshComponent(GameObject& aParent, std::shared_ptr<MeshAsset> aMesh, bool aShouldRenderLines, bool aIsAudioObstacle)
	: Component(aParent)
{
	myMesh = aMesh;
	myComponentType = ComponentType::Mesh;
	myRenderStages.at(RenderStage::ShadowMapping) = true;
	myRenderStages.at(RenderStage::Deferred) = true;
	myRenderStages.at(RenderStage::ObjectIDRendering) = true;
	if (aShouldRenderLines)
	{
		myParent.AddComponent(std::make_shared<BoxComponent>(myParent));
		myParent.GetLastAddedComponent<BoxComponent>()->Initialize(myMesh->GetMinPoint(), myMesh->GetMaxPoint(), aIsAudioObstacle);
	}
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
	CU::Matrix4x4f renderMatrix = myParent.GetTransform();
	if (myIgnoreParentScale)
	{
		renderMatrix = CU::Matrix4x4<float>::CreateRotationAroundZ(myParent.GetRotation().z * (3.14f / 180.0f));
		renderMatrix = CU::Matrix4x4<float>::CreateRotationAroundY(myParent.GetRotation().y * (3.14f / 180.0f)) * renderMatrix;
		renderMatrix = CU::Matrix4x4<float>::CreateRotationAroundX(myParent.GetRotation().x * (3.14f / 180.0f)) * renderMatrix;

		renderMatrix(4, 1) = myParent.GetPosition().x;
		renderMatrix(4, 2) = myParent.GetPosition().y;
		renderMatrix(4, 3) = myParent.GetPosition().z;
	}
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
				MainSingleton::Get().GetRenderer().Enqueue<GCmdRenderSkeletalMesh>(renderStage, myMesh, renderMatrix,
					this->GetParent().GetComponent<AnimationComponent>()->GetBoneTransforms(), myMaterials, myParent.GetID());
			}
			else
			{
				MainSingleton::Get().GetRenderer().Enqueue<GCmdRenderMesh>(renderStage, myMesh, renderMatrix, myMaterials, myParent.GetID());
			}
		}
	}
}

std::shared_ptr<MeshAsset> MeshComponent::GetMesh()
{
	return myMesh;
}

void MeshComponent::AddMaterial(std::shared_ptr<MaterialAsset> aMaterial)
{
	myMaterials.push_back(aMaterial);
}

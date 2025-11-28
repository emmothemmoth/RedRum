#include "GameEngine.pch.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "AnimationComponent.h"
#include "MainSingleton.h"

#include "../../AssetManager/AssetManager.h"
#include "GameObject.h"
#include "../../GraphicsEngine/Objects/MeshAsset.h"

#include "..\GraphicsEngine\Commands\GCmdRenderMesh.h"
#include "..\GraphicsEngine\Commands\GCmdRenderSkeletalMesh.h"

#include "..\GraphicsEngine\GraphicsEngine.h"

MeshComponent::MeshComponent(GameObject& aParent, std::shared_ptr<MeshAsset> aMesh)
	: Component(aParent)
{
	myMesh = aMesh;
	myID = ComponentID::Mesh;
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
	std::vector<std::shared_ptr<MaterialAsset>> materialList;
	if (this->GetParent().GetComponent<MaterialComponent>())
	{
		materialList = this->GetParent().GetComponent<MaterialComponent>()->GetMaterials();
	}
	else
	{
		materialList = GraphicsEngine::Get().GetDefaultMaterials();
	}
	if (this->GetParent().GetComponent<AnimationComponent>().get() != nullptr)
	{
		MainSingleton::Get().GetRenderer().Enqueue<GCmdRenderSkeletalMesh>(myMesh, myParent.GetTransform(),
			this->GetParent().GetComponent<AnimationComponent>()->GetBoneTransforms(),  materialList);
	}
	else
	{
		MainSingleton::Get().GetRenderer().Enqueue<GCmdRenderMesh>(myMesh, myParent.GetTransform(), materialList);
	}
}

std::shared_ptr<MeshAsset> MeshComponent::GetMesh()
{
	return myMesh;
}

#include "GameEngine.pch.h"
#include "GameObject.h"
#include "MeshComponent.h"
#include "BillboardComponent.h"
#include "../AssetManager/AssetManager.h"
#include "../GraphicsEngine/Objects/MeshAsset.h"
#include "../GraphicsEngine/Objects/MaterialAsset.h"
#include "../GraphicsEngine/Objects/TextureAsset.h"
#define DEFAULT_TRANSFORM_MESH "TransformGizmo"
#define DEFAULT_TRANSFORM_MATERIAL "M_TransformGizmoMaterial"
#define TRANSFORM_GIZMO_COMPONENT_ID 0
#define ICON_ID 1

GameObject::GameObject(std::string_view aName, unsigned anID)
{
	myID = anID;
	myName = aName;
	AddComponent(std::make_shared<MeshComponent>(*this, AssetManager::Get().GetAsset<MeshAsset>(DEFAULT_TRANSFORM_MESH), false));
	auto transformGizmo = GetLastAddedComponent<MeshComponent>();
	transformGizmo->SetRenderStage(RenderStage::ShadowMapping, false);
	transformGizmo->SetRenderStage(RenderStage::Deferred, false);
	transformGizmo->SetRenderStage(RenderStage::WorldSpaceUI);
	transformGizmo->SetRenderStage(RenderStage::ObjectPartIDRendering);
	transformGizmo->AddMaterial(AssetManager::Get().GetAsset<MaterialAsset>(DEFAULT_TRANSFORM_MATERIAL));
	transformGizmo->SetVisible(false);
	transformGizmo->SetIgnoreScale(true);
}

GameObject::GameObject(unsigned anID)
{
	myID = anID;
	myName = "Unnamed";
	AddComponent(std::make_shared<MeshComponent>(*this, AssetManager::Get().GetAsset<MeshAsset>(DEFAULT_TRANSFORM_MESH), false));
	auto transformGizmo = GetLastAddedComponent<MeshComponent>();
	transformGizmo->SetRenderStage(RenderStage::ShadowMapping, false);
	transformGizmo->SetRenderStage(RenderStage::Deferred, false);
	transformGizmo->SetRenderStage(RenderStage::WorldSpaceUI);
	transformGizmo->SetRenderStage(RenderStage::ObjectPartIDRendering);
	transformGizmo->AddMaterial(AssetManager::Get().GetAsset<MaterialAsset>(DEFAULT_TRANSFORM_MATERIAL));
	transformGizmo->SetVisible(false);
	transformGizmo->SetIgnoreScale(true);
}

GameObject::GameObject(std::string_view aName)
{
	myName = aName;
	AddComponent(std::make_shared<MeshComponent>(*this, AssetManager::Get().GetAsset<MeshAsset>(DEFAULT_TRANSFORM_MESH), false));
	auto transformGizmo = GetLastAddedComponent<MeshComponent>();
	transformGizmo->SetRenderStage(RenderStage::ShadowMapping, false);
	transformGizmo->SetRenderStage(RenderStage::Deferred, false);
	transformGizmo->SetRenderStage(RenderStage::WorldSpaceUI);
	transformGizmo->SetRenderStage(RenderStage::ObjectPartIDRendering);
	transformGizmo->AddMaterial(AssetManager::Get().GetAsset<MaterialAsset>(DEFAULT_TRANSFORM_MATERIAL));
	transformGizmo->SetVisible(false);
	transformGizmo->SetIgnoreScale(true);
}

GameObject::GameObject() = default;

GameObject::~GameObject()
{
}

void GameObject::Render()
{
	if (!myIsVisible) return;
	for (auto component : myComponents)
	{
 		component->Render();
	}
}

void GameObject::SetPosition(float anX, float aY, float aZ)
{
	myPosition = { anX, aY, aZ };
	myIsDirty = true;
}

void GameObject::SetPosition(const CU::Vector3f& aPosition)
{
	myPosition = aPosition;
	myIsDirty = true;
}

void GameObject::SetRotation(const CU::Vector3f& aRotation)
{
	myRotation = aRotation;
	myIsDirty = true;
}

void GameObject::SetScale(const CU::Vector3f& aScale)
{
	myScale = aScale;
	myIsDirty = true;
}


void GameObject::SetIcon(ComponentType aComponentType, const CU::Vector4f& anOffset)
{
	std::shared_ptr<BillboardComponent> icon = GetComponent<BillboardComponent>();
	if (!icon)
	{
		AddComponent(std::make_shared<BillboardComponent>(*this));
		icon = GetLastAddedComponent<BillboardComponent>();
	}
	icon->SetOffset(anOffset);
	switch (aComponentType)
	{
	case ComponentType::AudioSource:
		icon->SetTexture(AssetManager::Get().GetAsset<TextureAsset>("T_SourceIcon_C"));
		icon->SetVisible(true);
		return;
	case ComponentType::Listener:
		icon->SetTexture(AssetManager::Get().GetAsset<TextureAsset>("T_ListenerIcon_C"));
		icon->SetVisible(true);
		return;
	case ComponentType::Mesh:
	case ComponentType::MeshInstance:
		icon->SetTexture(AssetManager::Get().GetAsset<TextureAsset>("T_ObjectIcon_C"));
		icon->SetVisible(true);
		return;
	}
}

void GameObject::AddPosition(Gizmo_Axis anAxis, float aDelta)
{
	switch (anAxis)
	{
	case Gizmo_Axis::Gizmo_X: myPosition.x += aDelta; break;
	case Gizmo_Axis::Gizmo_Y: myPosition.y += aDelta; break;
	case Gizmo_Axis::Gizmo_Z: myPosition.z += aDelta; break;
	default: return;
	}

	myIsDirty = true;
}

void GameObject::AddRotation(Gizmo_Axis anAxis, float aDelta)
{
	switch (anAxis)
	{
	case Gizmo_Axis::Gizmo_X: myRotation.x += aDelta; break;
	case Gizmo_Axis::Gizmo_Y: myRotation.y += aDelta; break;
	case Gizmo_Axis::Gizmo_Z: myRotation.z += aDelta; break;
	default: return;
	}

	myIsDirty = true;
}

void GameObject::AddScale(Gizmo_Axis anAxis, float aDelta)
{
	switch (anAxis)
	{
	case Gizmo_Axis::Gizmo_X: myScale.x += aDelta; break;
	case Gizmo_Axis::Gizmo_Y: myScale.y += aDelta; break;
	case Gizmo_Axis::Gizmo_Z: myScale.z += aDelta; break;
	default: return;
	}

	myIsDirty = true;
}

const CU::Matrix4x4f& GameObject::GetTransform()
{
	UpdateTransform();
	return myTransform;
}

void GameObject::OnSelected()
{
	OnComponentSelected.Broadcast();
	myComponents.at(TRANSFORM_GIZMO_COMPONENT_ID)->SetVisible(true);
	myIsSelected = true;
}

void GameObject::OnDeselected()
{
	OnComponentDeselected.Broadcast();
	myComponents.at(TRANSFORM_GIZMO_COMPONENT_ID)->SetVisible(false);
	myIsSelected = false;
}

void GameObject::OnMove(Gizmo_Axis anAxis)
{
	anAxis;
	int a = 0;
	a++;
}

void GameObject::UpdateTransform()
{
	if (!myIsDirty) return;

	// 1. Scale
	CU::Matrix4x4<float> scaleMat;
	scaleMat(1, 1) = myScale.x;
	scaleMat(2, 2) = myScale.y;
	scaleMat(3, 3) = myScale.z;

	// 2. Rotation (Using your existing Euler logic)
	CU::Matrix4x4<float> rotMat;
	rotMat = CU::Matrix4x4<float>::CreateRotationAroundZ(myRotation.z * (3.14f / 180.0f)) * rotMat;
	rotMat = CU::Matrix4x4<float>::CreateRotationAroundY(myRotation.y * (3.14f / 180.0f)) * rotMat;
	rotMat = CU::Matrix4x4<float>::CreateRotationAroundX(myRotation.x * (3.14f / 180.0f)) * rotMat;

	// 3. Translation
	CU::Matrix4x4<float> transMat;
	transMat(4, 1) = myPosition.x;
	transMat(4, 2) = myPosition.y;
	transMat(4, 3) = myPosition.z;

	// Combine them: S * R * T
	myTransform = scaleMat * rotMat * transMat;
	myIsDirty = false;
}

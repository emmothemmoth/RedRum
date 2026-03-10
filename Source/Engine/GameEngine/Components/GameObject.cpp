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
	transformGizmo->AddMaterial(AssetManager::Get().GetAsset<MaterialAsset>(DEFAULT_TRANSFORM_MATERIAL));
	transformGizmo->SetVisible(false);
}

GameObject::GameObject(unsigned anID)
{
	myID = anID;
	myName = "Unnamed";
}

GameObject::GameObject(std::string_view aName)
{
	myName = aName;
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
	myTransform(4, 1) = anX;
	myTransform(4, 2) = aY;
	myTransform(4, 3) = aZ;
}

void GameObject::SetPosition(const CU::Vector3f& aPosition)
{
	myTransform(4, 1) = aPosition.x;
	myTransform(4, 2) = aPosition.y;
	myTransform(4, 3) = aPosition.z;
}

void GameObject::RotateAroundY(float anAngle)
{
	myTransform = CommonUtilities::Matrix4x4<float>::CreateRotationAroundY(anAngle * (3.14f / 180.0f)) * myTransform;
}

void GameObject::RotateAroundX(float anAngle)
{
	myTransform = CommonUtilities::Matrix4x4<float>::CreateRotationAroundX(anAngle * (3.14f / 180.0f)) * myTransform;
}

void GameObject::RotateAroundZ(float anAngle)
{
	myTransform = CommonUtilities::Matrix4x4<float>::CreateRotationAroundZ(anAngle * (3.14f / 180.0f)) * myTransform;
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
	 case ComponentType::Mesh:
	 case ComponentType::MeshInstance:
		 icon->SetTexture(AssetManager::Get().GetAsset<TextureAsset>("T_ObjectIcon_C"));
		 icon->SetVisible(true);
		 return;
	 }
}


void GameObject::OnSelected()
{
	OnComponentSelected.Broadcast();
	myComponents.at(TRANSFORM_GIZMO_COMPONENT_ID)->SetVisible(true);
}

void GameObject::OnDeselected()
{
	OnComponentDeselected.Broadcast();
	myComponents.at(TRANSFORM_GIZMO_COMPONENT_ID)->SetVisible(false);
}
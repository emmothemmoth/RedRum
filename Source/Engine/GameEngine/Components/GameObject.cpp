#include "GameEngine.pch.h"
#include "GameObject.h"

#include "MeshComponent.h"


GameObject::GameObject(std::string_view aName, unsigned anID)
{
	myID = anID;
	myName = aName;
}

GameObject::GameObject() = default;

GameObject::~GameObject()
{
}

//void GameObjectComponent::Update(const float& aDeltatime)
//{
//	//Bugg i animationplayer
//	GameObject::Update(aDeltatime);
//}


void GameObject::Render()
{
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

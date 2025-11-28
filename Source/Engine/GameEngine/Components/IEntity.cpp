#include "GameEngine.pch.h"
#include "IEntity.h"
#include "Component.h"
#include <assert.h>


IEntity::IEntity()
{
}

IEntity::~IEntity()
{

	myComponents.clear();
}

void IEntity::Update(const float& aDeltaTime)
{
	for (auto &component : myComponents)
	{
		component->Update(aDeltaTime);
	}
}

void IEntity::Render()
{
}


void IEntity::AddComponent(std::shared_ptr<Component> aComponent)
{
	assert(aComponent != nullptr && "A component can't be nullptr!");
	myComponents.push_back(aComponent);
}

CommonUtilities::Matrix4x4<float> IEntity::GetTransform()
{
	return myTransform;
}

CommonUtilities::Vector3<float> IEntity::GetPosition()
{
	CommonUtilities::Vector3<float> position;
	position.x = myTransform(4, 1);
	position.y = myTransform(4, 2);
	position.z = myTransform(4, 3);
	return position;
}

void IEntity::SetTransform(CommonUtilities::Matrix4x4<float>& aTransform)
{
	myTransform = aTransform;
}







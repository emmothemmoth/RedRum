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
	if (!myIsActive)
	{
		return;
	}
	for (auto& component : myComponents)
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
	myComponents.back()->SetID(myComponentIDCounter++);
}





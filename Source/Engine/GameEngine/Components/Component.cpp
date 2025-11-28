#include "GameEngine.pch.h"
#include "Component.h"

#include "GameObject.h"

Component::Component(GameObject& aParent) : myParent(aParent)
,myID(ComponentID::Count)
{
}

Component::~Component()
{
}

void Component::Update(const float aDeltaTime)
{
	aDeltaTime;
}

void Component::Render()
{
}

GameObject& Component::GetParent()
{
	return myParent;
}

ComponentID Component::GetID()
{
	return myID;
}




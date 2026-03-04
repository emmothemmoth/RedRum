#include "GameEngine.pch.h"
#include "Component.h"

#include "GameObject.h"

Component::Component(GameObject& aParent) : myParent(aParent)
,myComponentType(ComponentType::Count)
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




#pragma once
#include "ComponentID.h"

class GameObject;

class Component
{
public:
	Component(GameObject& aParent);
	virtual				~Component();
	virtual void		Update(const float aDeltaTime);
	virtual void Render();
	GameObject& GetParent();
	ComponentID GetID();

protected:
	GameObject& myParent;
	ComponentID myID;
};

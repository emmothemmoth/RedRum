#pragma once
#include "ComponentType.h"

class GameObject;

class Component
{
public:
	Component(GameObject& aParent);
	virtual				~Component();
	virtual void		Update(const float aDeltaTime);
	virtual void Render();

	void SetID(const unsigned anID) { myID = anID; }
	void SetVisible(const bool aIsVisible) { myIsVisible = aIsVisible; }
	void SetEnabled(const bool aIsEnabled) { myIsVisible = aIsEnabled; }

	GameObject& GetParent() { return myParent; }
	ComponentType GetComponentType() { return myComponentType; }
	unsigned GetID() { return myID; }

protected:
	GameObject& myParent;
	ComponentType myComponentType;
	unsigned myID;
	bool myIsVisible = true;
	bool myIsEnabled = true;
};

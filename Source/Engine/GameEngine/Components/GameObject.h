#pragma once
#include "IEntity.h"
#include "Component.h"
#include "GameObjectID.h"
#include "../Events/MulticastDelegate.h"

#include <string_view>

using FOnComponentSelected = MulticastDelegate<>;
using FOnComponentDeselected = MulticastDelegate<>;
class GameObject : public IEntity
{
public:
	GameObject(std::string_view aName, unsigned anID);
	GameObject(unsigned anID);
	GameObject(std::string_view aName);
	GameObject();
	~GameObject();

	//void Update(const float& aDeltatime) override;
	void Render() override;

	void SetPosition(float anX, float aY, float aZ);
	void SetPosition(const CU::Vector3f& aPosition);
	void RotateAroundY(float anAngle);
	void RotateAroundX(float anAngle);
	void RotateAroundZ(float anAngle);

	void OnSelected();
	void OnDeselected();

	unsigned SetID(const unsigned anID) { myID = anID; }
	unsigned GetID() const { return myID; }


public:
	FOnComponentSelected OnComponentSelected;
	FOnComponentDeselected OnComponentDeselected;

private:
	unsigned myID = 0;
	std::string myName;
	bool myIsSelected = false;
};


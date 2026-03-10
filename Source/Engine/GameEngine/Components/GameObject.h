#pragma once
#include "IEntity.h"
#include "Component.h"
#include "GameObjectID.h"
#include "../Events/MulticastDelegate.h"

#include <string_view>
enum class Gizmo_Axis : uint8_t
{
	Gizmo_X,
	Gizmo_Y,
	Gizmo_Z
};

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
	void SetIcon(ComponentType aComponentType, const CU::Vector4f& anOffset = {0.0f, 0.0f, 0.0f, 0.0f});

	unsigned SetID(const unsigned anID) { myID = anID; }
	unsigned GetID() const { return myID; }

	std::string_view GetName() const { return myName; }

	void OnSelected();
	void OnDeselected();

	void OnMove(Gizmo_Axis anAxis);
public:
	FOnComponentSelected OnComponentSelected;
	FOnComponentDeselected OnComponentDeselected;

private:
	unsigned myID = 0;
	std::string myName;
	bool myIsSelected = false;
};


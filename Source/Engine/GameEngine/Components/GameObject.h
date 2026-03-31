#pragma once
#include "IEntity.h"
#include "Component.h"
#include "GameObjectID.h"
#include "../Events/MulticastDelegate.h"

#include "CommonUtilities/Matrix4x4.hpp"
#include "CommonUtilities/Vector3.hpp"

#include <string_view>
enum class Gizmo_Axis : uint8_t
{
	None = 0,
	Gizmo_X = 1,
	Gizmo_Y = 2,
	Gizmo_Z = 3
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
	void SetTransform(const CU::Matrix4x4f& aTransform) { myTransform = aTransform; }
	void SetPosition(float anX, float aY, float aZ);
	void SetPosition(const CU::Vector3f& aPosition);
	void SetRotation(const CU::Vector3f& aRotation);
	void SetScale(const CU::Vector3f& aScale);
	void SetIcon(ComponentType aComponentType, const CU::Vector4f& anOffset = {0.0f, 0.0f, 0.0f, 0.0f});
	void SetID(const unsigned anID) { myID = anID; }

	void AddPosition(Gizmo_Axis anAxis, float aDelta);
	void AddRotation(Gizmo_Axis anAxis, float aDelta);
	void AddScale(Gizmo_Axis anAxis, float aDelta);

	unsigned GetID() const { return myID; }
	const CU::Matrix4x4f& GetTransform();
	std::string_view GetName() const { return myName; }
	const CU::Vector3f& GetPosition() const { return myPosition; }
	const CU::Vector3f& GetRotation() const { return myRotation; }
	const CU::Vector3f& GetScale() const { return myScale; }

	bool IsSelected() const { return myIsSelected; }
	bool IsDirty() const { return myIsDirty; }

	void OnSelected();
	void OnDeselected();

	void OnMove(Gizmo_Axis anAxis);

private:
	void UpdateTransform();
public:
	FOnComponentSelected OnComponentSelected;
	FOnComponentDeselected OnComponentDeselected;

private:
	std::string myName;
	CU::Matrix4x4f myTransform;
	CU::Vector3f myPosition = { 0.0f, 0.0f, 0.0f };
	CU::Vector3f myRotation = { 0.0f, 0.0f, 0.0f };
	CU::Vector3f myScale = { 1.0f, 1.0f, 1.0f };
	unsigned myID = 0;
	bool myIsSelected = false;
	bool myIsDirty = false;
};


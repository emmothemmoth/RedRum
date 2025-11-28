#pragma once
#include "IEntity.h"
#include "Component.h"
#include "GameObjectID.h"
#include <string_view>



class GameObject : public IEntity
{
public:
	GameObject(std::string_view aName, unsigned anID);
	GameObject();
	~GameObject();

	//void Update(const float& aDeltatime) override;
	void Render() override;

	void SetPosition(float anX, float aY, float aZ);
	void RotateAroundY(float anAngle);
	void RotateAroundX(float anAngle);
	void RotateAroundZ(float anAngle);

private:
	unsigned myID;
	std::string myName;
};


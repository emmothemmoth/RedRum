#pragma once
#include "Component.h"

#include "CommonUtilities\Matrix4x4.hpp"


class ControllerComponent : public Component
{
public:
	ControllerComponent(GameObject& aParent);
	~ControllerComponent();

	void Update(const float aDeltaTime) override;

	void RotateAroundY(float anAngle);
	void RotateAroundX(float anAngle);
	void RotateAroundZ(float anAngle);
private:
	CommonUtilities::Matrix4x4<float> myPositionDelta;
	float mySpeed;
	float myRotationSpeed;
};


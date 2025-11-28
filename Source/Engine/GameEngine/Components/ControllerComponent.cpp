#include "GameEngine.pch.h"
#include "ControllerComponent.h"

#include "CommonUtilities\Input.h"

#include "GameObject.h"

using namespace CommonUtilities;

ControllerComponent::ControllerComponent(GameObject& aParent) : Component(aParent)
{
	mySpeed = 200.0f;
	myRotationSpeed = 100.0f;
}

ControllerComponent::~ControllerComponent()
{
}


void ControllerComponent::Update(const float aDeltaTime)
{
	myPositionDelta = Matrix4x4<float>();
	//J
	if (Input::GetKeyHeld(Keys::J))
	{
		myPositionDelta(4,1) = -1.0f * aDeltaTime * mySpeed;
	}
	//L
	if (Input::GetKeyHeld(Keys::L))
	{
		myPositionDelta(4, 1) = 1.0f * aDeltaTime * mySpeed;
	}
	//O
	if (Input::GetKeyHeld(Keys::O))
	{
		myPositionDelta(4, 2) = 1.0f * aDeltaTime * mySpeed;
	}
	//U
	if (Input::GetKeyHeld(Keys::U))
	{
		myPositionDelta(4, 2) = -1.0f * aDeltaTime * mySpeed;
	}
	//I
	if (Input::GetKeyHeld(Keys::I))
	{
		myPositionDelta(4, 3) = 1.0f * aDeltaTime * mySpeed;
	}
	//K
	if (Input::GetKeyHeld(Keys::K))
	{
		myPositionDelta(4, 3) = -1.0f * aDeltaTime * mySpeed;
	}

	//rotate left - Y
	if (Input::GetKeyHeld(Keys::LEFT))
	{
		RotateAroundY(-1.0f * aDeltaTime * myRotationSpeed);
	}
	//rotate right - P
	if (Input::GetKeyHeld(Keys::RIGHT))
	{
		RotateAroundY(1.0f * aDeltaTime * myRotationSpeed);
	}
	//rotate Up - n
	if (Input::GetKeyHeld(Keys::UP))
	{
		RotateAroundX(1.0f * aDeltaTime * myRotationSpeed);
	}
	//rotate Down - M
	if (Input::GetKeyHeld(Keys::DOWN))
	{
		RotateAroundZ(-1.0f* aDeltaTime * myRotationSpeed);
	}
	Matrix4x4<float> move = myPositionDelta * myParent.GetTransform();
	myParent.SetTransform(move);
}

void ControllerComponent::RotateAroundY(float anAngle)
{
	myPositionDelta = Matrix4x4<float>::CreateRotationAroundY(anAngle * (3.14f / 180.0f)) * myPositionDelta;
}

void ControllerComponent::RotateAroundX(float anAngle)
{
	myPositionDelta =Matrix4x4<float>::CreateRotationAroundX(anAngle * (3.14f / 180.0f)) * myPositionDelta;
}

void ControllerComponent::RotateAroundZ(float anAngle)
{
	myPositionDelta = Matrix4x4<float>::CreateRotationAroundZ(anAngle * (3.14f / 180.0f)) * myPositionDelta;
}

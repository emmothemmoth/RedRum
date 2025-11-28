#include "GameEngine.pch.h"
#include "CameraComponent.h"
#include "MainSingleton.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include "CommonUtilities\InputMapper.h"
#include "..\GraphicsEngine\Commands\GraphicsCommandList.h"
#include "..\GraphicsEngine\Commands\GCmdSetFrameBuffer.h"
#include "..\GraphicsEngine\Buffers\FrameBuffer.h"

#include "../GraphicsEngine/GraphicsEngine.h"


using namespace CommonUtilities;

CameraComponent::CameraComponent(GameObject& aParent)
	:Component(aParent)
{
	myIsRotating = false;
	mySpeed = 500.f;
	myRotationSpeed = 20.0f;
	myRotationY = 0.0f;
	myRotationX = 0.0f;
	myResolution.x = 1600;
	myResolution.y = 900;
	myHorizontalFov = static_cast<float> (70.0f * (M_PI / 180.0f));
	myNearPlaneZ = 0.1f;
	myFarPlaneZ = 10000.0f;

}
CameraComponent::~CameraComponent()
{
	MainSingleton::Get().GetInputMapper().UnRegister(ActionEventID::CameraMove_Backwards, this);
	MainSingleton::Get().GetInputMapper().UnRegister(ActionEventID::CameraMove_Down, this);
	MainSingleton::Get().GetInputMapper().UnRegister(ActionEventID::CameraMove_Forward, this);
	MainSingleton::Get().GetInputMapper().UnRegister(ActionEventID::CameraMove_Left, this);
	MainSingleton::Get().GetInputMapper().UnRegister(ActionEventID::CameraMove_Right, this);
	MainSingleton::Get().GetInputMapper().UnRegister(ActionEventID::CameraMove_Up, this);
	MainSingleton::Get().GetInputMapper().UnRegister(ActionEventID::CameraRotation, this);
}


void CameraComponent::Init(CU::Vector3<float> aPoint)
{
	myFrameBuffer = std::make_shared<FrameBufferData>();
	myPosition.x = aPoint.x;
	myPosition.y = aPoint.y;
	myPosition.z = aPoint.z;
	myPosition.w = 1;

	float scaleX = 1 / static_cast<float>(tan(myHorizontalFov * 0.5f));
	float scaleY = (myResolution.x / myResolution.y) * scaleX;
	float planesDiv = myFarPlaneZ / (myFarPlaneZ - myNearPlaneZ);
	myRotationMatrix = Matrix4x4<float>::CreateRotationAroundY(myRotationY * static_cast<float>((M_PI) / 180.0f));
	myViewTransform = {};
	myViewTransform = myRotationMatrix;
	myViewTransform(4, 1) = myPosition.x;
	myViewTransform(4, 2) = myPosition.y;
	myViewTransform(4, 3) = myPosition.z;
	myViewTransform(4, 4) = 0;
	UpdateRotation();

	myClipMatrix(1, 1) = scaleX;
	myClipMatrix(1, 2) = 0;
	myClipMatrix(1, 3) = 0;
	myClipMatrix(1, 4) = 0;
	myClipMatrix(2, 1) = 0;
	myClipMatrix(2, 2) = scaleY;
	myClipMatrix(2, 3) = 0;
	myClipMatrix(2, 4) = 0;
	myClipMatrix(3, 1) = 0;
	myClipMatrix(3, 2) = 0;
	myClipMatrix(3, 3) = planesDiv;
	myClipMatrix(3, 4) = 1.0f;
	myClipMatrix(4, 1) = 0;
	myClipMatrix(4, 2) = 0;
	myClipMatrix(4, 3) = (-1 * planesDiv) * myNearPlaneZ;
	myClipMatrix(4, 4) = 0.0f;

	MainSingleton::Get().GetInputMapper().Register(ActionEventID::CameraMove_Backwards, this);
	MainSingleton::Get().GetInputMapper().Register(ActionEventID::CameraMove_Down, this);
	MainSingleton::Get().GetInputMapper().Register(ActionEventID::CameraMove_Forward, this);
	MainSingleton::Get().GetInputMapper().Register(ActionEventID::CameraMove_Left, this);
	MainSingleton::Get().GetInputMapper().Register(ActionEventID::CameraMove_Right, this);
	MainSingleton::Get().GetInputMapper().Register(ActionEventID::CameraMove_Up, this);
	MainSingleton::Get().GetInputMapper().Register(ActionEventID::CameraRotation, this);
}

void CameraComponent::Update(float aDeltaTime)
{
	Move(myDirection, aDeltaTime);

	if (myIsRotating)
	{
		POINT mouseDelta = Input::GetMousePositionDelta();
		myRotationY += mouseDelta.x * aDeltaTime;
		myRotationX += mouseDelta.y * aDeltaTime;
		UpdateRotation();
		myIsRotating = false;
	}


	myViewTransform(4, 1) = myPosition.x;
	myViewTransform(4, 2) = myPosition.y;
	myViewTransform(4, 3) = myPosition.z;
	myDirection = Vector4<float>(0, 0, 0, 0);
}

void CameraComponent::Render()
{
	myFrameBuffer->CameraPosition = myPosition;
	myFrameBuffer->Projection = GetClipMatrix();
	myFrameBuffer->View = GetViewInverse();
	myFrameBuffer->Resolution = GraphicsEngine::Get().GetViewPortSize();
	MainSingleton::Get().GetRenderer().Enqueue<GCmdSetFrameBuffer>(myFrameBuffer);
}

void CameraComponent::UpdateRotation()
{
	CU::Vector4<float> position;
	position.x = myViewTransform(4, 1);
	position.y = myViewTransform(4, 2);
	position.z = myViewTransform(4, 3);
	myViewTransform = CU::Matrix4x4<float>::CreateRotationAroundX(myRotationX * myRotationSpeed * static_cast<float> (M_PI / 180.0f)) * CU::Matrix4x4<float>::CreateRotationAroundY(myRotationY * myRotationSpeed * static_cast<float> (M_PI / 180.0f));
	myViewTransform(4, 1) = position.x;
	myViewTransform(4, 2) = position.y;
	myViewTransform(4, 3) = position.z;
}

void CameraComponent::Move(const CU::Vector4<float>& aDirection, const float& aDeltaTime)
{
	UNREFERENCED_PARAMETER(aDirection);
	myPosition += (myDirection * mySpeed * aDeltaTime) * myViewTransform;
}


CU::Vector4<float> CameraComponent::WorldToClip(const CU::Vector4<float>& aPoint)
{
	Vector4<float> cameraPoint = ToCamera(aPoint);

	Vector4<float> clipPoint = ToClip(cameraPoint);

	clipPoint.x = clipPoint.x / clipPoint.w;
	clipPoint.y = clipPoint.y / clipPoint.w;
	clipPoint.z = clipPoint.z / clipPoint.w;

	if (clipPoint.w == 0)
	{
		clipPoint.w += 0.0001f;
	}
	return clipPoint;
}

CU::Matrix4x4<float> CameraComponent::WorldToClip(const CU::Matrix4x4<float>& aTransform)
{
	CU::Vector4<float> point;
	point.x = aTransform(4, 1);
	point.y = aTransform(4, 2);
	point.z = aTransform(4, 3);
	CU::Vector4<float> cameraPoint = ToCamera(point);

	CU::Vector4<float> clipPoint = ToClip(cameraPoint);
	CU::Matrix4x4<float> transform;
	transform(4, 1) = clipPoint.x;
	transform(4, 2) = clipPoint.y;
	transform(4, 3) = clipPoint.z;
	return transform;
}

CU::Matrix4x4<float> CameraComponent::GetViewInverse()
{
	return CU::Matrix4x4<float>::GetFastInverse(myViewTransform);
}

CU::Matrix4x4<float> CameraComponent::GetViewMatrix()
{
	return myViewTransform;
}

CU::Matrix4x4<float> CameraComponent::GetClipMatrix()
{
	return myClipMatrix;
}

CU::Vector4<float> CameraComponent::GetPosition()
{
	return CU::Vector4<float>(myViewTransform(4, 1), myViewTransform(4, 2), myViewTransform(4, 3), myViewTransform(4,4));
}

void CameraComponent::SetSpeed(const float aSpeed)
{
	mySpeed = aSpeed;
}


void CameraComponent::RecieveEvent(const ActionEvent& anEvent)
{
	switch (anEvent.Id)
	{
	case ActionEventID::CameraMove_Left:
		myDirection = Vector4<float>(-1, 0, 0, 0);
		break;
	case ActionEventID::CameraMove_Right:
		myDirection = Vector4<float>(1, 0, 0, 0);
		break;
	case ActionEventID::CameraMove_Forward:
		myDirection = Vector4<float>(0, 0, 1, 0);
		break;
	case ActionEventID::CameraMove_Backwards:
		myDirection = Vector4<float>(0, 0, -1, 0);
		break;
	case ActionEventID::CameraMove_Up:
		myDirection = Vector4<float>(0, 1, 0, 0);
		break;
	case ActionEventID::CameraMove_Down:
		myDirection = Vector4<float>(0, -1, 0, 0);
		break;
	case ActionEventID::CameraRotation:
		myIsRotating = true;
		break;
	default:
		break;
	}
}


CU::Vector4<float> CameraComponent::ToCamera(const CU::Vector4<float>& aWorldPoint)
{
	CU::Matrix4x4<float> transformInverse = Matrix4x4<float>::GetFastInverse(myViewTransform);
	return aWorldPoint * transformInverse;
}

CU::Vector4<float> CameraComponent::ToClip(const CU::Vector4<float>& aCameraPoint)
{
	return aCameraPoint * myClipMatrix;
}




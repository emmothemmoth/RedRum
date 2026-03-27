#include "GameEngine.pch.h"
#include "CameraComponent.h"
#include "GameObject.h"
#include "MainSingleton.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include "CommonUtilities\InputMapper.h"
#include "..\GraphicsEngine\Commands\GraphicsCommandList.h"
#include "..\GraphicsEngine\Commands\GCmdSetFrameBuffer.h"
#include "..\GraphicsEngine\Commands\GCmdScreenPicking.h"
#include "..\GraphicsEngine\Buffers\FrameBuffer.h"

#include "../GraphicsEngine/GraphicsEngine.h"


using namespace CommonUtilities;

CameraComponent::CameraComponent(GameObject& aParent)
	:Component(aParent)
{
	myRenderStages.at(RenderStage::ShadowMapping) = false;
	myRenderStages.at(RenderStage::Deferred) = false;
	myRenderStages.at(RenderStage::WorldSpaceUI) = false;
	myComponentType = ComponentType::Camera;
	myParent.OnDeselected();
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
	MainSingleton::Get().GetInputMapper().UnRegister(ActionEventID::SelectObject, this);
	MainSingleton::Get().GetInputMapper().UnRegister(ActionEventID::MoveObject, this);
}


void CameraComponent::Init(CU::Vector3<float> aPoint, const CU::Vector3f& someRotations)
{
	myFrameBuffer = std::make_shared<FrameBufferData>();
	myPosition.x = aPoint.x;
	myPosition.y = aPoint.y;
	myPosition.z = aPoint.z;
	myPosition.w = 1;

	myRotationX = someRotations.x;
	myRotationY = someRotations.y;

	float scaleX = 1 / static_cast<float>(tan(myHorizontalFov * 0.5f));
	float scaleY = (myResolution.x / myResolution.y) * scaleX;
	float planesDiv = myFarPlaneZ / (myFarPlaneZ - myNearPlaneZ);

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
	MainSingleton::Get().GetInputMapper().Register(ActionEventID::SelectObject, this);
	MainSingleton::Get().GetInputMapper().Register(ActionEventID::MoveObject, this);
}

void CameraComponent::Update(float aDeltaTime)
{
	if (!myIsEnabled) return;
	myPosition = { myParent.GetPosition().x, myParent.GetPosition().y, myParent.GetPosition().z, 1.0f };
	Move(myDirection, aDeltaTime);
	myRotationX = myParent.GetRotation().x;
	myRotationY = myParent.GetRotation().y;

	if (myIsRotating)
	{
		POINT mouseDelta = Input::GetMousePositionDelta();
		myParent.AddRotation(Gizmo_Axis::Gizmo_Y, mouseDelta.x * aDeltaTime * myRotationSpeed);
		myParent.AddRotation(Gizmo_Axis::Gizmo_X, mouseDelta.y * aDeltaTime* myRotationSpeed);
		myIsRotating = false;
		if (mouseDelta.x > 0)
		{
			int a = 0;
			a++;
		}
	}
	GetParent().SetPosition({ myPosition.x, myPosition.y, myPosition.z });
	myDirection = Vector4<float>(0, 0, 0, 0);
}

void CameraComponent::Render()
{
	if (!myIsVisible) return;
	myFrameBuffer->CameraPosition = myPosition;
	myFrameBuffer->CameraRight = myParent.GetTransform().GetRow(1);
	myFrameBuffer->CameraUp = myParent.GetTransform().GetRow(2);
	myFrameBuffer->Projection = GetClipMatrix();
	myFrameBuffer->View = GetViewInverse();
	myFrameBuffer->Resolution = GraphicsEngine::Get().GetRenderSize();
	MainSingleton::Get().GetRenderer().Enqueue<GCmdSetFrameBuffer>(RenderStage::Deferred, myFrameBuffer);
}


void CameraComponent::Move(const CU::Vector4<float>& aDirection, const float& aDeltaTime)
{
	UNREFERENCED_PARAMETER(aDirection);
	myPosition += (myDirection * mySpeed * aDeltaTime) * myParent.GetTransform();
}

void CameraComponent::SetResolution(const CU::Vector2f& aResolution)
{
	myResolution = aResolution;
	UpdateProjection();
}

CU::Matrix4x4<float> CameraComponent::GetViewInverse()
{
	return CU::Matrix4x4<float>::GetFastInverse(myParent.GetTransform());
}

CU::Matrix4x4<float> CameraComponent::GetViewMatrix()
{
	return myParent.GetTransform();;
}

CU::Matrix4x4<float> CameraComponent::GetClipMatrix()
{
	return myClipMatrix;
}

void CameraComponent::SetSpeed(const float aSpeed)
{
	mySpeed = aSpeed;
}

void CameraComponent::RecieveEvent(const ActionEvent& anEvent)
{
	if (!myIsEnabled) return;
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
void CameraComponent::UpdateProjection()
{
	// Prevent division by zero if the viewport is completely hidden/minimized
	if (myResolution.y <= 0.0001f) return;

	float scaleX = 1 / static_cast<float>(tan(myHorizontalFov * 0.5f));
	float scaleY = (myResolution.x / myResolution.y) * scaleX;
	float planesDiv = myFarPlaneZ / (myFarPlaneZ - myNearPlaneZ);

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
}




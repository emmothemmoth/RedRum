#pragma once
#include "../Utilities/CommonUtilities\Vector.hpp"
#include "../Utilities/CommonUtilities\Matrix.hpp"
#include "../Utilities/CommonUtilities\InputObserver.h"
#include "Component.h"

#include "../Events/MulticastDelegate.h"

#include <array>

using FOnObjectClicked = MulticastDelegate<uint32_t>;
using FOnObjectDragged = MulticastDelegate<uint32_t>;
struct FrameBufferData;

class CameraComponent : public InputObserver, public Component
{
public:
	CameraComponent(GameObject& aParent);
	~CameraComponent();

	void Init(CU::Vector3<float> aPoint, const CU::Vector3f& someRotations = CU::Vector3f());
	void Update(float aDeltaTime) override;
	void Render() override;
	void UpdateRotation();
	void Move(const CU::Vector4<float>& aDirection, const float& aDeltaTime);
	void SetResolution(const CU::Vector2f& aResolution);
	CU::Vector4<float> WorldToClip(const CU::Vector4<float>& aPoint);
	CU::Matrix4x4<float> WorldToClip(const CU::Matrix4x4<float>& aTransform);
	CU::Matrix4x4<float> GetViewInverse();
	CU::Matrix4x4<float> GetViewMatrix();
	CU::Matrix4x4<float> GetClipMatrix();
	CU::Vector4<float> GetPosition();
	void SetSpeed(const float aSpeed);

	void RecieveEvent(const ActionEvent& anEvent);
	FOnObjectClicked OnObjectClicked;

private:
	CU::Vector4<float> ToCamera(const CU::Vector4<float>& aWorldPoint);
	CU::Vector4<float> ToClip(const CU::Vector4<float>& aCameraPoint);
	void UpdateProjection();

	std::shared_ptr<FrameBufferData> myFrameBuffer;

	CU::Matrix4x4<float> myRotationMatrix;
	CU::Vector4<float> myPosition;
	CU::Vector4<float> myDirection;
	CU::Matrix4x4<float> myTransform;
	CU::Matrix4x4<float> myViewTransform;
	CU::Matrix4x4<float> myClipMatrix;
	float mySpeed;
	float myRotationSpeed;
	float myRotationY;
	float myRotationX;
	float myHorizontalFov;
	CU::Vector2<float> myResolution;
	float myNearPlaneZ;
	float myFarPlaneZ;
	bool myIsRotating;
};




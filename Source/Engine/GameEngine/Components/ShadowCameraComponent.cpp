#include "GameEngine.pch.h"
#include "ShadowCameraComponent.h"
#include "MainSingleton.h"

#include "..\GraphicsEngine\Buffers\FrameBuffer.h"

#include "..\GraphicsEngine\GraphicsEngine.h"
#include "..\GraphicsEngine\Commands\GCmdSetFrameBuffer.h"


ShadowCameraComponent::ShadowCameraComponent(GameObject& aParent)
	: Component(aParent)
{
}

void ShadowCameraComponent::Init(const CU::Matrix4x4<float>& aView, const CU::Matrix4x4<float>& aProjection, const CU::Vector3<float>& aPosition)
{
	myFrameBuffer = std::make_shared<FrameBufferData>();
	myFrameBuffer->CameraPosition.x = aPosition.x;
	myFrameBuffer->CameraPosition.y = aPosition.y;
	myFrameBuffer->CameraPosition.z = aPosition.z;
	myFrameBuffer->Projection = aProjection;
	myFrameBuffer->View = aView;
}

void ShadowCameraComponent::Render()
{
	MainSingleton::Get().GetRenderer().Enqueue<GCmdSetFrameBuffer>(myFrameBuffer);
}

std::shared_ptr<FrameBufferData> ShadowCameraComponent::GetFrameBuffer()
{
	return myFrameBuffer;
}

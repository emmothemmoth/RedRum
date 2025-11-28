#include "GameEngine.pch.h"
#include "LightComponent.h"
#include "..\GraphicsEngine\Commands\GCmdSetLightBuffer.h"
#include "MainSingleton.h"

#include "CommonUtilities\InputMapper.h"
#include "CommonUtilities\Input.h"
#include "CommonUtilities\Vector.hpp"
#include "CommonUtilities\Matrix4x4.hpp"
#include "WorldBounds.h"


LightComponent::LightComponent(GameObject& aParent) : Component(aParent)
{
	myLightBuffer = std::make_shared<LightBuffer>();
	MainSingleton::Get().GetInputMapper().Register(ActionEventID::Toggle_DirectionalLight, this);
	MainSingleton::Get().GetInputMapper().Register(ActionEventID::Toggle_PointLights, this);
	MainSingleton::Get().GetInputMapper().Register(ActionEventID::Toggle_SpotLights, this);
}

LightComponent::~LightComponent()
{
	MainSingleton::Get().GetInputMapper().UnRegister(ActionEventID::Toggle_DirectionalLight, this);
	MainSingleton::Get().GetInputMapper().UnRegister(ActionEventID::Toggle_PointLights, this);
	MainSingleton::Get().GetInputMapper().UnRegister(ActionEventID::Toggle_SpotLights, this);
}

void LightComponent::Update(const float aDeltaTime)
{
	UNREFERENCED_PARAMETER(aDeltaTime);
}

void LightComponent::InitDirLight(const WorldBounds& aWorld, const CU::Vector3f& aColor, float aIntensity, const CU::Vector4f& aLightDirection)
{
	myLightBuffer->DirLight.Color = aColor;
	myLightBuffer->DirLight.Intensity = aIntensity;
	myLightBuffer->DirLight.LightDir = aLightDirection;
	CU::Vector4f origin = { aWorld.Origin.x, aWorld.Origin.y, aWorld.Origin.z, 1.0f };
	CU::Vector4f lightPos = origin + (-2.0f * aWorld.Radius * aLightDirection);
	myLightBuffer->DirLight.LightPos = lightPos;
	myLightBuffer->DirLight.Active = true;
	const CommonUtilities::Vector3<float> targetPos = aWorld.Origin;
	const CommonUtilities::Vector3<float> globalUpDir = CommonUtilities::Vector3<float>({ 0.0f, 1.0f, 0.0f });
	const CU::Vector3f pos = CU::Vector3f(lightPos.x, lightPos.y, lightPos.z);
	const CommonUtilities::Matrix4x4<float> lightView = CommonUtilities::Matrix4x4<float>::LookAt(pos, targetPos, globalUpDir);
	myLightBuffer->DirLight.LightViewInv = CU::Matrix4x4<float>::GetFastInverse(lightView);
	const CommonUtilities::Vector3<float> frustrumCenter = CU::Vector3f({ aWorld.Origin.x, aWorld.Origin.y, aWorld.Radius});
	const float leftPlane = frustrumCenter.x - aWorld.Radius;
	const float bottomPlane = frustrumCenter.y - aWorld.Radius;
	const float nearPlane = frustrumCenter.z - aWorld.Radius * 2.0f;
	const float rightPlane = frustrumCenter.x + aWorld.Radius;
	const float topPlane = frustrumCenter.y + aWorld.Radius;
	const float farPlane = frustrumCenter.z + aWorld.Radius * 2.0f;
	myLightBuffer->DirLight.LightProj = CU::Matrix4x4<float>::CreateOrthoGraphicProjection(leftPlane, rightPlane, bottomPlane, topPlane, nearPlane, farPlane);

}

void LightComponent::InitPointLight(const int anIndex, const CU::Vector3f& aColor, float aIntensity, float aRange, const CU::Vector3f& aLightPosition, CU::Matrix4x4f aLightProj, CU::Matrix4x4f aLightView)
{
	myLightBuffer->PointLights[anIndex].CastShadows = true;
	myLightBuffer->PointLights[anIndex].Color = aColor;
	myLightBuffer->PointLights[anIndex].Intensity = aIntensity;
	myLightBuffer->PointLights[anIndex].LightProj = aLightProj;
	myLightBuffer->PointLights[anIndex].LightViewInv = aLightView;
	myLightBuffer->PointLights[anIndex].LightPos = aLightPosition;
	myLightBuffer->PointLights[anIndex].Range = aRange;
	myLightBuffer->PointLights[anIndex].Active = true;
}

void LightComponent::InitSpotLight(const int anIndex, const CU::Vector3f& aColor, float aIntensity, float aRange, const CU::Vector3f& aLightDirection, const CU::Vector3f& aLightPosition, float aInnerConeAngle, float anOuterConeAngle, CU::Matrix4x4f aLightProj, CU::Matrix4x4f aLightView)
{
	myLightBuffer->SpotLights[anIndex].CastShadows = true;
	myLightBuffer->SpotLights[anIndex].Color = aColor;
	myLightBuffer->SpotLights[anIndex].Intensity = aIntensity;
	myLightBuffer->SpotLights[anIndex].LightDir = aLightDirection;
	myLightBuffer->SpotLights[anIndex].LightPos = aLightPosition;
	myLightBuffer->SpotLights[anIndex].LightProj = aLightProj;
	myLightBuffer->SpotLights[anIndex].LightViewInv = aLightView;
	myLightBuffer->SpotLights[anIndex].InnerConeAngle = aInnerConeAngle;
	myLightBuffer->SpotLights[anIndex].OuterConeAngle = anOuterConeAngle;
	myLightBuffer->SpotLights[anIndex].Range = aRange;
	myLightBuffer->SpotLights[anIndex].Active = true;
}

std::shared_ptr<LightBuffer> LightComponent::GetLightBuffer() const
{
	return myLightBuffer;
}

void LightComponent::Render()
{
	MainSingleton::Get().GetRenderer().Enqueue<GCmdSetLightBuffer>(myLightBuffer);
}

void LightComponent::RecieveEvent(const ActionEvent& anEvent)
{
	switch (anEvent.Id)
	{
	case ActionEventID::Toggle_DirectionalLight:
		if (myLightBuffer->DirLight.Active)
		{
			myLightBuffer->DirLight.Active = false;
			break;
		}
		myLightBuffer->DirLight.Active = true;
		break;
	case ActionEventID::Toggle_PointLights:
		if (myLightBuffer->PointLights[0].Active)
		{
			myLightBuffer->PointLights[0].Active = false;
			break;
		}
		myLightBuffer->PointLights[0].Active = true;
		break;
	case ActionEventID::Toggle_SpotLights:
		if (myLightBuffer->SpotLights[0].Active)
		{
			myLightBuffer->SpotLights[0].Active = false;
			break;
		}
		myLightBuffer->SpotLights[0].Active = true;
		break;
	default:
		break;
	}
}

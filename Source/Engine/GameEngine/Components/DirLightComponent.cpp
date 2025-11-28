#include "GameEngine.pch.h"
#include "DirLightComponent.h"

#include "WorldBounds.h"

#include "MainSingleton.h"

DirLightComponent::DirLightComponent(GameObject& aParent)
	: Component(aParent)
{
	myLightData = std::make_shared<LightBuffer::DirLightData>();
	MainSingleton::Get().GetInputMapper().Register(ActionEventID::Toggle_DirectionalLight, this);
}

DirLightComponent::~DirLightComponent()
{
	MainSingleton::Get().GetInputMapper().UnRegister(ActionEventID::Toggle_DirectionalLight, this);
}

void DirLightComponent::InitDirLight(const WorldBounds& aWorld, const CU::Vector3f& aColor, float aIntensity, const CU::Vector4f& aLightDirection)
{
	myLightData->Color = aColor;
	myLightData->Intensity = aIntensity;
	myLightData->LightDir = aLightDirection;
	CU::Vector4f origin = { aWorld.Origin.x, aWorld.Origin.y, aWorld.Origin.z, 1.0f };
	CU::Vector4f lightPos = origin + (-2.0f * aWorld.Radius * aLightDirection);
	myLightData->LightPos = lightPos;
	myLightData->Active = true;
	const CommonUtilities::Vector3<float> targetPos = aWorld.Origin;
	const CommonUtilities::Vector3<float> globalUpDir = CommonUtilities::Vector3<float>({ 0.0f, 1.0f, 0.0f });
	const CU::Vector3f pos = CU::Vector3f(lightPos.x, lightPos.y, lightPos.z);
	const CommonUtilities::Matrix4x4<float> lightView = CommonUtilities::Matrix4x4<float>::LookAt(pos, targetPos, globalUpDir);
	myLightData->LightViewInv = CU::Matrix4x4<float>::GetFastInverse(lightView);
	const CommonUtilities::Vector3<float> frustrumCenter = CU::Vector3f({ aWorld.Origin.x, aWorld.Origin.y, aWorld.Radius });
	const float leftPlane = frustrumCenter.x - aWorld.Radius;
	const float bottomPlane = frustrumCenter.y - aWorld.Radius;
	const float nearPlane = frustrumCenter.z - aWorld.Radius * 2.0f;
	const float rightPlane = frustrumCenter.x + aWorld.Radius;
	const float topPlane = frustrumCenter.y + aWorld.Radius;
	const float farPlane = frustrumCenter.z + aWorld.Radius * 2.0f;
	myLightData->LightProj = CU::Matrix4x4<float>::CreateOrthoGraphicProjection(leftPlane, rightPlane, bottomPlane, topPlane, nearPlane, farPlane);
}

std::shared_ptr<LightBuffer::DirLightData> DirLightComponent::GetLightData() const
{
	return myLightData;
}


void DirLightComponent::RecieveEvent(const ActionEvent& anEvent)
{
	switch (anEvent.Id)
	{
	case ActionEventID::Toggle_DirectionalLight:
		if (myLightData->Active)
		{
			myLightData->Active = false;
			break;
		}
		myLightData->Active = true;
		return;
	default:
		return;
	}
}

#include "GameEngine.pch.h"
#include "SpotLightComponent.h"

#include "MainSingleton.h"



SpotLightComponent::SpotLightComponent(GameObject& aParent)
	: Component(aParent)
{
	myLightData = std::make_shared<LightBuffer::SpotLightData>();
	MainSingleton::Get().GetInputMapper().Register(ActionEventID::Toggle_SpotLights, this);
}

SpotLightComponent::~SpotLightComponent()
{
	MainSingleton::Get().GetInputMapper().UnRegister(ActionEventID::Toggle_SpotLights, this);
}

void SpotLightComponent::InitSpotLight(const int anIndex, const CU::Vector3f& aColor, float aIntensity, float aRange, const CU::Vector3f& aLightDirection, const CU::Vector3f& aLightPosition, float aInnerConeAngle, float anOuterConeAngle, CU::Matrix4x4f aLightProj, CU::Matrix4x4f aLightViewInverse)
{
	UNREFERENCED_PARAMETER(anIndex);
	myLightData->CastShadows = true;
	myLightData->Color = aColor;
	myLightData->Intensity = aIntensity;
	myLightData->LightDir = aLightDirection;
	myLightData->LightPos = aLightPosition;
	myLightData->LightProj = aLightProj;
	myLightData->LightViewInv = aLightViewInverse;
	myLightData->InnerConeAngle = aInnerConeAngle;
	myLightData->OuterConeAngle = anOuterConeAngle;
	myLightData->Range = aRange;
	myLightData->Active = true;
}

std::shared_ptr<LightBuffer::SpotLightData> SpotLightComponent::GetLightData() const
{
	return myLightData;
}

void SpotLightComponent::RecieveEvent(const ActionEvent& anEvent)
{
	switch (anEvent.Id)
	{
	case ActionEventID::Toggle_SpotLights:
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

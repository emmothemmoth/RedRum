#include "GameEngine.pch.h"
#include "PointLightComponent.h"

#include "MainSingleton.h"




PointLightComponent::PointLightComponent(GameObject& aParent)
	: Component(aParent)
{
	myLightData = std::make_shared<LightBuffer::PointLightData>();
	MainSingleton::Get().GetInputMapper().Register(ActionEventID::Toggle_PointLights, this);
}

PointLightComponent::~PointLightComponent()
{
	MainSingleton::Get().GetInputMapper().UnRegister(ActionEventID::Toggle_PointLights, this);
}

void PointLightComponent::InitPointLight(const int anIndex, const CU::Vector3f& aColor, float aIntensity, float aRange, const CU::Vector3f& aLightPosition, CU::Matrix4x4f aLightProj, CU::Matrix4x4f aLightViewInverse)
{
	UNREFERENCED_PARAMETER(anIndex);
	myLightData->CastShadows = true;
	myLightData->Color = aColor;
	myLightData->Intensity = aIntensity;
	myLightData->LightProj = aLightProj;
	myLightData->LightViewInv = aLightViewInverse;
	myLightData->LightPos = aLightPosition;
	myLightData->Range = aRange;
	myLightData->Active = true;
}

std::shared_ptr<LightBuffer::PointLightData> PointLightComponent::GetLightData() const
{
	return myLightData;
}

void PointLightComponent::RecieveEvent(const ActionEvent& anEvent)
{
	switch (anEvent.Id)
	{
	case ActionEventID::Toggle_PointLights:
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

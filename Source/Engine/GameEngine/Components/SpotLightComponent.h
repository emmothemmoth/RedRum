#pragma once

#include "Component.h"

#include "CommonUtilities\InputObserver.h"
#include "..\GraphicsEngine\Buffers\LightBuffer.h"

#include <memory>

class Vector3;
class Matrix4x4;


class SpotLightComponent : public Component, public InputObserver
{
public:
	SpotLightComponent(GameObject& aParent);
	~SpotLightComponent();

	void InitSpotLight(const int anIndex, const CU::Vector3f& aColor, float aIntensity, float aRange, const CU::Vector3f& aLightDirection,
		const CU::Vector3f& aLightPosition, float aInnerConeAngle, float anOuterConeAngle, CU::Matrix4x4f aLightProj, CU::Matrix4x4f aLightViewInverse);

	std::shared_ptr<LightBuffer::SpotLightData> GetLightData() const;

	void RecieveEvent(const ActionEvent& anEvent);

private:
	std::shared_ptr<LightBuffer::SpotLightData> myLightData;
};

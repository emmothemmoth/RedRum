#pragma once


#include "Component.h"

#include "CommonUtilities\InputObserver.h"
#include "..\GraphicsEngine\Buffers\LightBuffer.h"

#include <memory>

class Vector3;
class Matrix4x4;

class PointLightComponent : public Component, public InputObserver
{
public:
	PointLightComponent(GameObject& aParent);
	~PointLightComponent();

	void InitPointLight(const int anIndex, const CU::Vector3f& aColor, float aIntensity, float aRange,
		const CU::Vector3f& aLightPosition, CU::Matrix4x4f aLightProj, CU::Matrix4x4f aLightViewInverse);

	std::shared_ptr<LightBuffer::PointLightData> GetLightData() const;

	void RecieveEvent(const ActionEvent& anEvent);

private:
	std::shared_ptr<LightBuffer::PointLightData> myLightData;
};


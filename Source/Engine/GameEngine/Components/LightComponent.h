#pragma once

#include "CommonUtilities\Matrix4x4.hpp"
#include "Component.h"
#include "../GraphicsEngine\Buffers\LightBuffer.h"
#include "CommonUtilities\InputObserver.h"

//#include <memory>


struct WorldBounds;


class LightComponent : public Component, public InputObserver
{
public:
	LightComponent(GameObject& aParent);
	~LightComponent();

	void Update(const float aDeltaTime) override;

	void InitDirLight(const WorldBounds& aWorld, const CU::Vector3f& aColor, float aIntensity, const CU::Vector4f& aLightDirection);

	void InitPointLight(const int anIndex, const CU::Vector3f& aColor, float aIntensity, float aRange,
		const CU::Vector3f& aLightPosition, CU::Matrix4x4f aLightProj, CU::Matrix4x4f aLightView);

	void InitSpotLight(const int anIndex, const CU::Vector3f& aColor, float aIntensity, float aRange, const CU::Vector3f& aLightDirection,
		const CU::Vector3f& aLightPosition, float aInnerConeAngle, float anOuterConeAngle, CU::Matrix4x4f aLightProj, CU::Matrix4x4f aLightView);

	std::shared_ptr<LightBuffer> GetLightBuffer() const;

	void Render();

	void RecieveEvent(const ActionEvent& anEvent);

private:

private:
	std::shared_ptr<LightBuffer> myLightBuffer;

};

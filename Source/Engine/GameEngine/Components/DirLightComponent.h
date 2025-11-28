#pragma once

#include "Component.h"

#include "CommonUtilities\InputObserver.h"
#include "..\GraphicsEngine\Buffers\LightBuffer.h"

#include <memory>

struct WorldBounds;
class Vector3;
class Vector4;

class DirLightComponent : public Component, public InputObserver
{
public:
	DirLightComponent(GameObject& aParent);
	~DirLightComponent();

	void InitDirLight(const WorldBounds& aWorld, const CU::Vector3f& aColor, float aIntensity, const CU::Vector4f& aLightDirection);

	std::shared_ptr<LightBuffer::DirLightData> GetLightData() const;

	void RecieveEvent(const ActionEvent& anEvent);

private:
	std::shared_ptr<LightBuffer::DirLightData> myLightData;
};

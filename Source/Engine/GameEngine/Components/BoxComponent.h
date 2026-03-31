#pragma once

#include "Component.h"

#include "CommonUtilities/AABB3D.hpp"

#include "../GraphicsEngine/Objects/DebugLineObject.h"

#include "../../AudioEngine/RoomSimulator/Collider.h"
#include "../../AudioEngine/RoomSimulator/AbsorberSettings.h"
#include "../../AudioEngine/AudioEngine.h"

#include <memory>

class MeshAsset;

class BoxComponent : public Component
{
public:
	BoxComponent(GameObject& aParent);
	~BoxComponent();

	void Initialize(const CU::Vector3f aMin, const CU::Vector3f aMax, bool aIsAudioObstacle = false);
	void Update(const float aDeltaTime) override;
	void Render() override;

	bool IsVisible() const { return myIsVisible; }

	AbsorberSettings& GetAbsorberSettings() { return myAbsorberSettings; }

private:
	void ShowLines();
	void HideLines();

private:
	CU::AABB3D<float> myBox;
	AbsorberSettings myAbsorberSettings;
	Collider myCollider;
	std::shared_ptr<DebugLineObject> myDebugObject;
	ObstacleHandle myObstacleHandle = UINT32_MAX;
};



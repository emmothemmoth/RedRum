#pragma once

#include "Component.h"

#include "CommonUtilities/AABB3D.hpp"

#include "../GraphicsEngine/Objects/DebugLineObject.h"

#include <memory>

class MeshAsset;

class BoxComponent : public Component
{
public:
	BoxComponent(GameObject& aParent);
	~BoxComponent();

	void Initialize(const CU::Vector3f aMin, const CU::Vector3f aMax);
	void Render() override;

	bool IsVisible() const { return myIsVisible; }

private:
	void ShowLines();
	void HideLines();

private:
	CU::AABB3D<float> myBox;
	std::shared_ptr<DebugLineObject> myDebugObject;
};



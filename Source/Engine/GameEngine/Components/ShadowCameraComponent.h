#pragma once

#include "Component.h"
#include <memory>
#include "WorldBounds.h"
struct FrameBufferData;
class Vector3;
class Matrix4x4;

class ShadowCameraComponent : public Component
{
public:
	ShadowCameraComponent(GameObject& aParent);
	~ShadowCameraComponent() override = default;

	void Init(const CU::Matrix4x4<float>& aView, const CU::Matrix4x4<float>& aProjection, const CU::Vector3<float>& aPosition);

	void Render() override;

	std::shared_ptr<FrameBufferData> GetFrameBuffer();


private:
	std::shared_ptr<FrameBufferData> myFrameBuffer;
};




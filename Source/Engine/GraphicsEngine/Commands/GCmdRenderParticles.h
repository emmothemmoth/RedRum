#pragma once
#include "GraphicsCommandList.h"

#include "../Utilities/CommonUtilities/Matrix4x4.hpp"

class ParticleSystem;

class GCmdRenderParticles : public GraphicsCommandBase
{
public:
	GCmdRenderParticles(CU::Matrix4x4f aTransform, std::shared_ptr<ParticleSystem> aParticleSystem);
	~GCmdRenderParticles() override = default;

	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<ParticleSystem> myParticleSystem;
	CU::Matrix4x4f myTransform;
};
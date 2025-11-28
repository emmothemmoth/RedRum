#include "GraphicsEngine.pch.h"
#include "ParticleSystem.h"

#include "ParticleEmitter.h"

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Update(const float aDeltaTime)
{
	for (auto& emitter : myEmitters)
	{
		emitter->Update(aDeltaTime);
	}
}

const std::vector<std::shared_ptr<ParticleEmitter>>& ParticleSystem::GetEmitters() const
{
	return myEmitters;
}

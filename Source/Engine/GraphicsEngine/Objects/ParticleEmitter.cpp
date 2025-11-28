#include "GraphicsEngine.pch.h"
#include "ParticleEmitter.h"
#include "Objects\ParticleEmitterSettings.h"



ParticleEmitter::ParticleEmitter()
{
}

ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::Init(const ParticleEmitterSettings& someSettings)
{
	mySettings = std::make_shared<ParticleEmitterSettings>();
	*mySettings = someSettings;
	InitInternal();
	myVertexBuffer = std::make_shared<VertexBuffer>();
	myVertexBuffer->Name = "Particle vertex buffer";
	myVertexBuffer->VertexStride = sizeof(ParticleVertex);
	myVertexBuffer->NumVertices = myParticles.size();
	myVertexBuffer->DataSize = sizeof(ParticleVertex) * myVertexBuffer->NumVertices;
}


std::shared_ptr<ParticleEmitterSettings> ParticleEmitter::GetSettings() const
{
	return mySettings;
}

std::shared_ptr<VertexBuffer> ParticleEmitter::GetVertexBuffer()
{
	return myVertexBuffer;
}


const std::vector<ParticleVertex>& ParticleEmitter::GetParticleVertices() const
{
	return myParticles;
}

void ParticleEmitter::Update(const float aDeltaTime)
{
	for (int index = 0; index < myParticles.size(); index++)
	{
		myParticles[index].LifeTime += aDeltaTime;
		if (myParticles[index].LifeTime < 0.0f)
		{
			return;
		}
		if (myParticles[index].LifeTime >= mySettings->LifeTime)
		{
			InitParticle(myParticles[index], index);
		}
		UpdateParticle(myParticles[index], aDeltaTime);
	}
}

void ParticleEmitter::InitParticle(ParticleVertex& aParticle, const unsigned anIndex)
{
	const float spwnRateInv = 1.0f / mySettings->SpawnRate;
	aParticle.LifeTime = 0.0f - static_cast<float>(anIndex) * spwnRateInv;
	aParticle.Color = { 1.0f, 0.0f, 0.0f, 0.0f };
	aParticle.Velocity = { 0.0f, 0.0f, 0.0f };
	aParticle.Angle = 10.0f;
	aParticle.Position = { 0.0f, 100.0f, 0.0f, 1.0f };
	aParticle.Size = { 50.0f, 50.0f };
}

void ParticleEmitter::UpdateParticle(ParticleVertex& aParticle, const float aDeltaTime)
{
	aParticle.Position.x += aParticle.Velocity.x * aDeltaTime;
	aParticle.Position.y += aParticle.Velocity.y * aDeltaTime;
	aParticle.Position.z += aParticle.Velocity.z * aDeltaTime;

	aParticle.Velocity += CU::Vector3f{ 0.001f, 0.001f, 0.0f };
	//aParticle.Velocity += CU::Vector3f{ 0.0f, -9.81f, mySettings->GravityScale * aDeltaTime };
	aParticle.Angle += 90.0f * aDeltaTime;
	if (aParticle.Angle > 360.0f)
	{
		aParticle.Angle = 0.0f;
	}
}

void ParticleEmitter::InitInternal()
{
	mySettings = std::make_shared<ParticleEmitterSettings>();
	const size_t numParticles = static_cast<size_t>(ceilf(mySettings->SpawnRate * mySettings->LifeTime));
	myParticles.resize(numParticles);

	for (int index = 0; index < numParticles; index++)
	{
		InitParticle(myParticles[index], index);
	}
}

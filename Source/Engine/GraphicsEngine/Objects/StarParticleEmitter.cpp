#include "GraphicsEngine.pch.h"
#include "StarParticleEmitter.h"

#include "ParticleEmitterSettings.h"

#include "CommonUtilities\RandomWrapper.h"

StarParticleEmitter::StarParticleEmitter()
{
}

StarParticleEmitter::~StarParticleEmitter()
{
}

void StarParticleEmitter::Update(const float aDeltaTime)
{
	for (int index = 0; index < myParticles.size(); index++)
	{
		myParticles[index].LifeTime += aDeltaTime;
		if (myParticles[index].LifeTime < 0.0f)
		{
			continue;
		}
		if (myParticles[index].LifeTime >= mySettings->LifeTime)
		{
			InitParticle(myParticles[index], index);
		}
		UpdateParticle(myParticles[index], aDeltaTime);
	}
}

void StarParticleEmitter::InitParticle(ParticleVertex& aParticle, const unsigned anIndex)
{
	//Todo:: init depending on shape!
	CU::RandomWrapper random;
	const float spwnRateInv = 1.0f / mySettings->SpawnRate;
	aParticle.LifeTime = 0.0f - static_cast<float>(anIndex) * spwnRateInv;
	aParticle.Color = { random.GetRandomFloat(0.0f, 1.0f), random.GetRandomFloat(0.0f, 1.0f), random.GetRandomFloat(0.0f, 1.0f), 1.0f};
	aParticle.Size = { 10.0f, 10.0f };
	aParticle.Velocity = { random.GetRandomFloat(0.0f, mySettings->Velocity.x * 2)-mySettings->Velocity.x
		,random.GetRandomFloat(0.0f, mySettings->Velocity.y * 2) - mySettings->Velocity.y,
		random.GetRandomFloat(0.0f,mySettings->Velocity.z * 2)- mySettings->Velocity.z };
	aParticle.Angle = random.GetRandomFloat(0.0f, 360.0f);
	aParticle.Position = { random.GetRandomFloat(0.0f, 50.0f),random.GetRandomFloat(0.0f, 50.0f), random.GetRandomFloat(0.0f, 50.0f), 0.5f };
	aParticle.ChannelMask = { random.GetRandomFloat(0.0f, 1.0f), random.GetRandomFloat(0.0f, 1.0f), random.GetRandomFloat(0.0f, 1.0f), random.GetRandomFloat(0.0f, 1.0f) };
}

void StarParticleEmitter::UpdateParticle(ParticleVertex& aParticle, const float aDeltaTime)
{
	aParticle.Position.x += aParticle.Velocity.x * aDeltaTime;
	aParticle.Position.y += aParticle.Velocity.y * aDeltaTime;
	aParticle.Position.z += aParticle.Velocity.z * aDeltaTime;
	CullFromShape(aParticle);
	
	//aParticle.Angle += 90.0f * aDeltaTime;
	//if (aParticle.Angle > 360.0f)
	//{
	//	aParticle.Angle = 0.0f;
	//}
}

void StarParticleEmitter::CullFromShape(ParticleVertex& aParticle)
{
	float distanceSqr = (aParticle.Position - CU::Vector4f{ 0.0f, 0.0f, 0.0f, 1.0f }).LengthSqr();
	switch (mySettings->Shape)
	{
	case EmitterShape::Sphere:
		if (distanceSqr > mySettings->ShapeRadius * mySettings->ShapeRadius)
		{
			aParticle.LifeTime = mySettings->LifeTime;
		}
		break;
	case EmitterShape::Box:
		if (abs(aParticle.Position.x) >= mySettings->ShapeRadius || abs(aParticle.Position.y) >= mySettings->ShapeRadius || abs(aParticle.Position.z) >= mySettings->ShapeRadius)
		{
			aParticle.LifeTime = mySettings->LifeTime;
		}
		break;
	default:
		break;
	}
}

void StarParticleEmitter::InitInternal()
{
	const size_t numParticles = static_cast<size_t>(ceilf(mySettings->SpawnRate * mySettings->LifeTime));
	myParticles.resize(numParticles);

	for (int index = 0; index < numParticles; index++)
	{
		InitParticle(myParticles[index], index);
	}
}

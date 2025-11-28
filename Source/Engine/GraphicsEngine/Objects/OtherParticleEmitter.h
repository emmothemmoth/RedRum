#pragma once


#include "ParticleEmitter.h"


class OtherParticleEmitter : public ParticleEmitter
{
	friend class GraphicsEngine;
public:
	OtherParticleEmitter();
	~OtherParticleEmitter() override;

	void Update(const float aDeltaTime) override;

protected:
	void InitParticle(ParticleVertex& aParticle, const unsigned anIndex);
	void UpdateParticle(ParticleVertex& aParticle, const float aDeltaTime);
	void CullFromShape(ParticleVertex& aParticle);
	void InitInternal() override;


private:

};

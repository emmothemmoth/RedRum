#pragma once
#include "Component.h"

#include "../GraphicsEngine/Objects/ParticleType.h"

#include <filesystem>

class ParticleSystem;

class ParticleSystemComponent : public Component
{
public:
	ParticleSystemComponent(GameObject& aParent);
	~ParticleSystemComponent();

	void Init(ParticleEmitterType aType);
	bool Init(const std::filesystem::path& aFileNameWithExtension);
	void Update(const float aDeltaTime) override;
	void Render() override;

	std::shared_ptr<ParticleSystem> GetParticleSystem() const;

private:
	bool InitFromJSON(const std::filesystem::path& aPath);

private:
	std::shared_ptr<ParticleSystem> myParticleSystem;
};


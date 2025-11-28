#pragma once
#include <vector>

class ParticleEmitter;
struct ParticleEmitterSettings;
class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();
	void Update(const float aDeltaTime);

	//template<typename EmitterType, typename EmitterSettingsType>
	//void AddEmitter(const EmitterSettingsType& someSettings);

	template<typename EmitterType>
	void AddEmitter(const ParticleEmitterSettings& someSettings);

	const std::vector<std::shared_ptr<ParticleEmitter>>& GetEmitters()const;

private:
	std::vector<std::shared_ptr<ParticleEmitter>> myEmitters;
};

//template<typename EmitterType, typename EmitterSettingsType>
//inline void ParticleSystem::AddEmitter(const EmitterSettingsType& someSettings)
//{
//	std::shared_ptr<EmitterType> emitter = std::make_shared<EmitterType>();
//	emitter->Init(someSettings);
//	myEmitters.emplace_back(emitter);
//}

template<typename EmitterType>
inline void ParticleSystem::AddEmitter(const ParticleEmitterSettings& someSettings)
{
	std::shared_ptr<EmitterType> emitter = std::make_shared<EmitterType>();
	emitter->Init(someSettings);
	myEmitters.emplace_back(emitter);
}

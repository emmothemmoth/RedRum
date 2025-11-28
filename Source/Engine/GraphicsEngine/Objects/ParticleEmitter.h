#pragma once

#include "../GraphicsEngine/GraphicsEngine.h"
#include "ParticleVertex.h"

struct ParticleEmitterSettings;
struct VertexBuffer;

//TODO: This is just a baseclass: should be moved into a derived class!
//Emittersettings should be inherited

class ParticleEmitter
{
	friend class GraphicsEngine;
public:
	ParticleEmitter();
	virtual ~ParticleEmitter();

	//template<typename EmitterSettingsType>
	//std::enable_if_t<std::is_base_of_v<ParticleEmitterSettings, EmitterSettingsType>>
	//Init(const EmitterSettingsType& someSettings);

	void Init(const ParticleEmitterSettings& someSettings);

	//template<typename EmitterSettingsType>
	//std::shared_ptr<EmitterSettingsType> GetSettings() const;

	std::shared_ptr<ParticleEmitterSettings> GetSettings() const;

	std::shared_ptr<VertexBuffer> GetVertexBuffer();

	const std::vector<ParticleVertex>& GetParticleVertices() const;

	virtual void Update(const float aDeltaTime);

protected:
	virtual void InitParticle(ParticleVertex& aParticle, const unsigned anIndex);
	virtual void UpdateParticle(ParticleVertex& aParticle, const float aDeltaTime);
	virtual void InitInternal();

protected:
	std::vector<ParticleVertex> myParticles;
	std::shared_ptr<ParticleEmitterSettings> mySettings;
	std::shared_ptr<VertexBuffer> myVertexBuffer;

private:
	//std::shared_ptr<ParticleEmitterSettings> mySettings;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;

};

//template<typename EmitterSettingsType>
//inline void ParticleEmitter::Init(const EmitterSettingsType& someSettings)
//{
//	mySettings = std::make_shared<EmitterSettingsType>(someSettings);
//	*mySettings = someSettings;
//	InitInternal();
//}

//template<typename EmitterSettingsType>
//inline std::enable_if_t<std::is_base_of_v<ParticleEmitterSettings, EmitterSettingsType>> ParticleEmitter::Init(const EmitterSettingsType& someSettings)
//{
//	mySettings = std::make_shared<EmitterSettingsType>();
//	*mySettings = someSettings;
//	InitInternal();
//}

//template<typename EmitterSettingsType>
//inline std::shared_ptr<EmitterSettingsType> ParticleEmitter::GetSettings() const
//{
//	return std::dynamic_pointer_cast<EmitterSettingsType>(mySettings);
//}

#pragma once
#include "Component.h"
#include <filesystem>

#include "../AudioEngine/RoomSimulator/EmitterSettings.h"

class AudioEngine;

class AudioSourceComponent : public Component
{
public:
	AudioSourceComponent(GameObject& aParent);
	~AudioSourceComponent();

	void Init(const std::filesystem::path& aSourceFile);
	void Update(const float aDeltaTime) override;
	void Render() override;

	void Play();
	void Pause();
	void Stop();

	bool IsPlayable() const { return myIsPlayable; }

	const std::filesystem::path& GetAudioSourceName() const { return myFilePath; }
	EmitterSettings& GetSettings() { return mySettings; }

	void MarkDirty() { myIsDirty = true; }
private:
	std::filesystem::path myFilePath;
	EmitterSettings mySettings;
	uint32_t mySourceHandle = UINT32_MAX;
	uint32_t myEmitterHandle = UINT32_MAX;
	bool myIsPlayable = false;
	bool myIsDirty = false;
};
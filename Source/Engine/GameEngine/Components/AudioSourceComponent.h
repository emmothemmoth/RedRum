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

	void OnEditorChange();

	bool IsPlayable() const { return myIsPlayable; }

	const std::filesystem::path& GetAudioSourceName() const { return myFilePath; }
	EmitterSettings& GetSettings() { return mySettings; }
private:
	uint32_t myAudioHandle = UINT32_MAX;
	std::filesystem::path myFilePath;
	EmitterSettings mySettings;
	bool myIsPlayable = false;
};
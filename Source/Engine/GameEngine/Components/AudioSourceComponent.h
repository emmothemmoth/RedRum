#pragma once
#include "Component.h"
#include <filesystem>

class AudioEngine;

struct AudioSourceSettings
{
	float Volume = 1.0f;
};

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
	AudioSourceSettings& GetSettings() { return mySettings; }
private:
	uint32_t myAudioHandle = UINT32_MAX;
	std::filesystem::path myFilePath;
	AudioSourceSettings mySettings;
	bool myIsPlayable = false;
};
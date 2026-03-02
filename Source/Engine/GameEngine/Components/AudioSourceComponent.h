#pragma once
#include "Component.h"
#include <filesystem>

class AudioEngine;

class AudioSourceComponent : public Component
{
public:
	AudioSourceComponent(GameObject& aParent);
	~AudioSourceComponent();

	void Init(const std::filesystem::path& aSourceFile);
	void Update(const float aDeltaTime) override;
	void Render() override;

	void Play(); //TEMP
	void Pause();
	void Stop();

	void OnEditorChange();

	bool IsPlayable() const { return myIsPlayable; }
private:
	uint32_t myAudioHandle = UINT32_MAX;
	std::filesystem::path myFilePath;
	bool myIsPlayable = false;
};
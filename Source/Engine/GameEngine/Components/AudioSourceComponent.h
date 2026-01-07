#pragma once
#include "Component.h"
#include <filesystem>

class AudioSourceComponent : public Component
{
public:
	AudioSourceComponent(GameObject& aParent);
	~AudioSourceComponent();

	void Init(const std::filesystem::path& aSourceFile);

	void ManualUpdate();

	void Play();
	void Stop();


	bool IsPlayable() const { return myIsPlayable; }
private:
	uint32_t myAudioHandle = UINT32_MAX;
	std::filesystem::path myFilePath;
	bool myIsPlayable = false;
};
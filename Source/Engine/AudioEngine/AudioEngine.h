#pragma once

#include <memory>
#include <optional>
#include <filesystem>

#include "CommonUtilities/Matrix4x4.hpp"

namespace juce { class AudioDeviceManager; }
typedef uint32_t AudioHandle;

enum class AudiosourceControl
{
	Play,
	Pause,
	Stop
};

class AudioEngine
{
public:
	AudioEngine();
	~AudioEngine();

	void Initialize();

	void InitListener(const CommonUtilities::Matrix4x4f& aMatrix);
	//Reads provided file and returns an audio handle for control
	std::optional<AudioHandle> RegisterSoundSource(const std::filesystem::path& aFilePath);
	void UnregisterSoundSource();

	void UpdateSoundSource(const AudioHandle aHandle, const CU::Matrix4x4f& aMatrix);

	void Control2DSource(const AudioHandle aHandle, const AudiosourceControl aControltype);

	bool IsInitialized() const { return myIsInitialized; }

private:
	struct Impl;
	std::unique_ptr<Impl> myImpl;
	bool myIsInitialized = false;
};


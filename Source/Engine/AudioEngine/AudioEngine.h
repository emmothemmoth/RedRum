#pragma once

#include <memory>
#include <optional>
#include <filesystem>

#include "CommonUtilities/Matrix4x4.hpp"

namespace juce { class AudioDeviceManager; }
typedef uint32_t AudioHandle;

class AudioEngine
{
public:
	AudioEngine();
	~AudioEngine();

	void Initialize();

	void InitListener(const CommonUtilities::Matrix4x4f& aMatrix);
	std::optional<AudioHandle> RegisterSoundSource(const std::filesystem::path& aFilePath);
	void UnregisterSoundSource();

	void UpdateSoundSource(const AudioHandle, const CU::Matrix4x4f& aMatrix);

	bool IsInitialized() const { return myIsInitialized; }

private:
	struct Impl;
	std::unique_ptr<Impl> myImpl;
	bool myIsInitialized = false;
};


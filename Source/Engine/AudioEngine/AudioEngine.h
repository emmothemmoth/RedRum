#pragma once

#include <memory>
#include <optional>
#include <filesystem>

#include "CommonUtilities/Matrix4x4.hpp"

namespace juce { class AudioDeviceManager; }
struct AbsorberSettings;
struct EmitterSettings;
struct Collider;
typedef uint32_t AudioHandle;
typedef uint32_t EmitterHandle;
typedef uint32_t ObstacleHandle;

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
	//Reads provided file and returns an audio handle for control
	void InitListener(const CU::Matrix4x4f& aTransform);
	std::optional<AudioHandle> RegisterSoundSource(const std::filesystem::path& aFilePath);
	void UnregisterSoundSource(const AudioHandle aHandle);
	std::optional<EmitterHandle> RegisterAudioEmitter(AudioHandle aSourceHandle, const EmitterSettings& someSettings, const CU::Matrix4x4f& aTransform);
	std::optional<ObstacleHandle> RegisterAudioObstacle(const AbsorberSettings& someSettings, const Collider& aCollider, const CU::Matrix4x4f& aTransform);
	void UnregisterAudioObstacle(ObstacleHandle aHandle);

	void UpdateSoundSource(const AudioHandle aHandle, const CU::Matrix4x4f& aMatrix);

	void Control2DSource(const AudioHandle aHandle, const AudiosourceControl aControltype);

	bool IsInitialized() const { return myIsInitialized; }

private:
	struct Impl;
	std::unique_ptr<Impl> myImpl;
	bool myIsInitialized = false;
};


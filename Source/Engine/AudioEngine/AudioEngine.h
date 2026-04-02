#pragma once

#include "RoomSimulator/VisualRayPath.h"

#include <memory>
#include <optional>
#include <filesystem>

#include "CommonUtilities/Matrix4x4.hpp"


#include "../GameEngine/Events/MulticastDelegate.h"

namespace juce { class AudioDeviceManager; }
struct AbsorberSettings;
struct EmitterSettings;
struct Collider;
typedef uint32_t AudioHandle;
typedef uint32_t EmitterHandle;
typedef uint32_t ObstacleHandle;
using FOnSimulationReady = MulticastDelegate<AudioHandle>;
using FOnSimulationStarted = MulticastDelegate<>;
using FOnVisualRaysReady = MulticastDelegate<EmitterHandle, std::vector<VisualRayPath>>;

enum class BakeMode
{
	Preview_Fast,
	Preview,
	Export
};

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
	void Update();
	//Reads provided file and returns an audio handle for control
	void InitListener(const CU::Matrix4x4f& aTransform);
	void UpdateListener(const CU::Matrix4x4f& aTransform);
	std::optional<AudioHandle> RegisterSoundSource(const std::filesystem::path& aFilePath);
	void UnregisterSoundSource(const AudioHandle aHandle);
	std::optional<EmitterHandle> RegisterAudioEmitter(AudioHandle aSourceHandle, const EmitterSettings& someSettings, const CU::Matrix4x4f& aTransform);
	void UpdateAudioEmitter(const EmitterHandle aHandle, const EmitterSettings& someSettings, const CU::Matrix4x4f& aMatrix);
	void UnregisterEmitter(const EmitterHandle aHandle);
	std::optional<ObstacleHandle> RegisterAudioObstacle(const AbsorberSettings& someSettings, const Collider& aCollider, const CU::Matrix4x4f& aTransform);
	void UpdateAudioObstacle(ObstacleHandle aHandle, const AbsorberSettings& someSettings, const Collider& aCollider, const CU::Matrix4x4f& aTransform);
	void UnregisterAudioObstacle(ObstacleHandle aHandle);

	void Control2DSource(const AudioHandle aHandle, const AudiosourceControl aControltype);
	void ControlRoomPlayback(AudiosourceControl aControltype);

	void StartRoomSimulation();

	bool IsInitialized() const { return myIsInitialized; }
	const double& GetSampleRate() const { return mySampleRate; }
	void SetSampleRate(const double& aSampleRate);
	void SetBakeMode(BakeMode aMode);

	FOnSimulationStarted OnSimulationStarted;
	FOnSimulationReady OnSimulationReady;
	FOnVisualRaysReady OnVisualRaysReady;

private:
	struct Impl;
	std::unique_ptr<Impl> myImpl;
	double mySampleRate = 48000.0;
	bool myIsInitialized = false;
	bool myRoomIsBaked = false;
};


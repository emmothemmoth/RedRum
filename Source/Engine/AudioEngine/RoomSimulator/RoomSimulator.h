#pragma once
#include "EmitterSettings.h"
#include "AbsorberSettings.h"
#include "Collider.h"
#include "VisualRayPath.h"
#include "AcousticData.h"
#include "CommonUtilities/Matrix4x4.hpp"

#include "../GameEngine/Events/MulticastDelegate.h"

#include <vector>
#include <optional>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <juce_audio_basics/juce_audio_basics.h>

struct AudioObstacle
{
	CU::Matrix4x4f Transform;
	CU::Matrix4x4f InverseTransform; // CRITICAL for the local-space trick
	AbsorberSettings Absorber;
	Collider Collider;
	uint32_t ID;
};

struct AudioEmitter
{
	const juce::AudioBuffer<float>* SourceBuffer = nullptr;
	CU::Matrix4x4f Transform;
	EmitterSettings Settings;
	float SampleRate;
	uint32_t ID;
};

struct ComputeRequest
{
	std::vector<GPURay> Rays;
	std::vector<GPUObstacle> Obstacles;
	std::vector<CU::Vector3f> Probes;
	float ProbeRadius;
	CU::Vector3f ListenerRight;
	std::shared_ptr<std::promise<std::vector<GPUMegaHit>>> Promise;
};

class RoomSimulator
{
public:
	RoomSimulator();
	~RoomSimulator();
	void Update();
	void InitListener(const CU::Matrix4x4f& aTransform);
	void UpdateListener(const CU::Matrix4x4f& aTransform);
	std::optional<uint32_t> RegisterEmitter(const juce::AudioBuffer<float>* aSourceBuffer, float aSampleRate, const EmitterSettings& someSettings, const CU::Matrix4x4f& aTransform);
	void UpdateEmitter(const uint32_t aHandle, const EmitterSettings& someSettings, const CU::Matrix4x4f& aTransform);
	void UnregisterEmitter(const uint32_t aHandle);
	std::optional<uint32_t> RegisterObstacle(const AbsorberSettings& someSettings, const Collider& aCollider, const CU::Matrix4x4f& aTransform);
	void UpdateObstacle(const uint32_t aHandle, const AbsorberSettings& someSettings, const Collider& aCollider, const CU::Matrix4x4f& aTransform);
	void UnregisterObstacle(const uint32_t aHandle);

	void Simulate();

	MulticastDelegate<juce::AudioBuffer<float>> OnBakeComplete;
	MulticastDelegate<uint32_t, std::vector<VisualRayPath>> OnScoutBatchReady;
	MulticastDelegate<std::vector<AcousticProbe>> OnMegaBakeComplete;
	const juce::AudioBuffer<float>& GetSimulation() const { return mySimulation; }
	void SetBakeRate(const double& aBakeRate) { myBakeRate = aBakeRate; }
	void SetRayLimit(const int aLimit) { myRayLimit = aLimit; }

	CU::Matrix4x4f GetListenerTransform();

	std::vector<AudioEmitter> GetSourcesCopy();
	int GetRayLimit() const { return myRayLimit; }
private:
	void WorkerThreadLoop();

	std::vector<CU::Vector3f> GenerateGrid(CU::Vector3f minBounds, CU::Vector3f maxBounds, float spacing);


private:
	std::vector<AudioObstacle> myObstacles;
	std::vector<AudioEmitter> mySources;
	juce::AudioBuffer<float> mySimulation;
	uint32_t myObstacleIDCounter = 0;
	uint32_t myEmitterCounter = 0;
	CU::Matrix4x4f myListener;

	std::thread myWorkerThread;
	std::mutex myMutex;
	std::condition_variable myConditionVariable;

	std::mutex myComputeMutex;
	std::optional<ComputeRequest> myPendingComputeRequest = std::nullopt;
	double myBakeRate = 48000;
	int myRayLimit = 500;
	float myProbeSpacing = 200.0f;
	float myProbeRadius = 50.0f;
	bool myHasWork = false;
	bool myShouldExit = false;
};


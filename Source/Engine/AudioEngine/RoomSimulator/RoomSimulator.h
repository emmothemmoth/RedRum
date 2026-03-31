#pragma once
#include "EmitterSettings.h"
#include "AbsorberSettings.h"
#include "Collider.h"

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
	AbsorberSettings Absorber;
	Collider Collider;
	//Absorber settings, transmission etc
	uint32_t ID;
};

struct AudioEmitter
{
	const juce::AudioBuffer<float>* SourceBuffer = nullptr;
	CU::Matrix4x4f Transform;
	EmitterSettings Settings;
	uint32_t ID;
};

class RoomSimulator
{
public:
	RoomSimulator();
	~RoomSimulator();
	void InitListener(const CU::Matrix4x4f& aTransform);
	void UpdateListener(const CU::Matrix4x4f& aTransform);
	std::optional<uint32_t> RegisterEmitter(const juce::AudioBuffer<float>* aSourceBuffer, const EmitterSettings& someSettings, const CU::Matrix4x4f& aTransform);
	void UpdateEmitter(const uint32_t aHandle, const EmitterSettings& someSettings, const CU::Matrix4x4f& aTransform);
	void UnregisterEmitter(const uint32_t aHandle);
	std::optional<uint32_t> RegisterObstacle(const AbsorberSettings& someSettings, const Collider& aCollider, const CU::Matrix4x4f& aTransform);
	void UpdateObstacle(const AbsorberSettings& someSettings, const Collider& aCollider, const CU::Matrix4x4f& aTransform);
	void UnregisterObstacle(const uint32_t aHandle);

	void Simulate();

	MulticastDelegate<juce::AudioBuffer<float>> OnBakeComplete;
	const juce::AudioBuffer<float>& GetSimulation() const { return mySimulation; }

private:
	void WorkerThreadLoop();


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

	bool myHasWork = false;
	bool myShouldExit = false;
};


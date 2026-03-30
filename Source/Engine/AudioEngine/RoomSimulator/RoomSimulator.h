#pragma once
#include "EmitterSettings.h"
#include "AbsorberSettings.h"
#include "Collider.h"

#include "CommonUtilities/Matrix4x4.hpp"

#include <vector>
#include <optional>

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
	CU::Matrix4x4f Transform;
	EmitterSettings Settings;
	uint32_t ID;
};

class RoomSimulator
{
public:
	void InitListener(const CU::Matrix4x4f& aTransform);
	std::optional<uint32_t> RegisterEmitter(const juce::AudioBuffer<float>& aSourceBuffer, const EmitterSettings& someSettings, const CU::Matrix4x4f& aTransform);
	void UnregisterEmitter(const uint32_t aHandle);
	std::optional<uint32_t> RegisterObstacle(const AbsorberSettings& someSettings, const Collider& aCollider, const CU::Matrix4x4f& aTransform);
	void UnregisterObstacle(const uint32_t aHandle);

	//On it's own thread?
	void Simulate();

	//const juce::AudioBuffer<float>& GetSimulation() const { return mySimulation; }


private:
	std::vector<AudioObstacle> myObstacles;
	std::vector<AudioEmitter> mySources;
	juce::AudioBuffer<float> mySimulation;
	uint32_t myObstacleIDCounter = 0;
	uint32_t myEmitterCounter = 0;
	CU::Matrix4x4f myListener;
};


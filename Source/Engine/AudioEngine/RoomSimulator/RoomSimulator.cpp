#include "RoomSimulator.h"

void RoomSimulator::InitListener(const CU::Matrix4x4f& aTransform)
{
	myListener = aTransform;
}

std::optional<uint32_t> RoomSimulator::RegisterEmitter(const juce::AudioBuffer<float>& aSourceBuffer, const EmitterSettings& someSettings, const CU::Matrix4x4f& aTransform)
{
	aSourceBuffer;
	someSettings;
	aTransform;
	return myEmitterCounter++;
}

void RoomSimulator::UnregisterEmitter(const uint32_t aHandle)
{
	aHandle;
}

std::optional<uint32_t> RoomSimulator::RegisterObstacle(const AbsorberSettings& someSettings, const Collider& aCollider, const CU::Matrix4x4f& aTransform)
{
	someSettings;
	aCollider;
	aTransform;
	return myObstacleIDCounter++;
}

void RoomSimulator::UnregisterObstacle(const uint32_t aHandle)
{
	aHandle;
}

void RoomSimulator::Simulate()
{
}

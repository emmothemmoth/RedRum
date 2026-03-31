#include "RoomSimulator.h"

#include <cmath>
#include <random>

RoomSimulator::RoomSimulator()
{
	myWorkerThread = std::thread(&RoomSimulator::WorkerThreadLoop, this);
}

RoomSimulator::~RoomSimulator()
{
	{
		std::lock_guard<std::mutex> lock(myMutex);
		myShouldExit = true;
	}
	myConditionVariable.notify_one();
	if (myWorkerThread.joinable())
	{
		myWorkerThread.join();
	}
}

void RoomSimulator::InitListener(const CU::Matrix4x4f& aTransform)
{
	myListener = aTransform;
}

void RoomSimulator::UpdateListener(const CU::Matrix4x4f& aTransform)
{
    std::lock_guard<std::mutex> lock(myMutex);
    myListener = aTransform;
}

std::optional<uint32_t> RoomSimulator::RegisterEmitter(const juce::AudioBuffer<float>* aSourceBuffer, const EmitterSettings& someSettings, const CU::Matrix4x4f& aTransform)
{
	std::lock_guard<std::mutex> lock(myMutex);

	AudioEmitter newEmitter;
	newEmitter.SourceBuffer = aSourceBuffer;
	newEmitter.Settings = someSettings;
	newEmitter.Transform = aTransform;
	newEmitter.ID = myEmitterCounter++;

	mySources.push_back(newEmitter);
	return newEmitter.ID;
}

void RoomSimulator::UpdateEmitter(const uint32_t aHandle, const EmitterSettings& someSettings, const CU::Matrix4x4f& aTransform)
{
    std::lock_guard<std::mutex> lock(myMutex);
    for (auto& emitter : mySources)
    {
        if (emitter.ID == aHandle)
        {
            emitter.Settings = someSettings;
            emitter.Transform = aTransform;
            break;
        }
    }
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

void RoomSimulator::UpdateObstacle(const AbsorberSettings& someSettings, const Collider& aCollider, const CU::Matrix4x4f& aTransform)
{
	someSettings;
	aCollider;
	aTransform;
}

void RoomSimulator::UnregisterObstacle(const uint32_t aHandle)
{
	aHandle;
}

void RoomSimulator::Simulate()
{
	{
		std::lock_guard<std::mutex> lock(myMutex);
		myHasWork = true;
	}
	myConditionVariable.notify_one();
}


void RoomSimulator::WorkerThreadLoop()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(myMutex);

		myConditionVariable.wait(lock, [this] { return myHasWork || myShouldExit; });

		if (myShouldExit) break;

		myHasWork = false;

		std::vector<AudioEmitter> localSources = mySources;
		CU::Matrix4x4f localListener = myListener;

		lock.unlock();

		int maxSamples = 0;
		for (const auto& emitter : localSources)
		{
			if (emitter.SourceBuffer != nullptr)
			{
				int sampleCount = emitter.SourceBuffer->getNumSamples();
				maxSamples = sampleCount > maxSamples ? sampleCount : maxSamples;
			}
		}

		juce::AudioBuffer<float> finishedBake(2, maxSamples);
		finishedBake.clear();

		CU::Vector3f listenerPos = { localListener(4,1), localListener(4,2), localListener(4,3) };
		CU::Vector3f listenerRight = { localListener(1,1), localListener(1,2), localListener(1,3) };
		listenerRight.Normalize();

		std::mt19937 rng(42); 
		std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

		const int SCOUT_RAY_COUNT = 100;
		//const int HEAVY_RAY_COUNT = 9900;

		for (const auto& emitter : localSources)
		{
			if (!emitter.SourceBuffer || emitter.SourceBuffer->getNumSamples() == 0)
				continue;

			CU::Vector3f sourcePos = { emitter.Transform(4,1), emitter.Transform(4,2), emitter.Transform(4,3) };

			std::vector<VisualRayPath> scoutPaths;
			scoutPaths.reserve(SCOUT_RAY_COUNT);

			for (int i = 0; i < SCOUT_RAY_COUNT; ++i)
			{
				CU::Vector3f randomDir(dist(rng), dist(rng), dist(rng));
				randomDir.Normalize();

				VisualRayPath path;
				RayBounce firstBounce;
				firstBounce.StartPos = sourcePos;
				firstBounce.StartPower = 1.0f;

				// TODO: Replace this mock distance with actual PhysicsEngine::Raycast!
				float mockHitDistance = 500.0f + (dist(rng) * 200.0f);

				firstBounce.EndPos = sourcePos + (randomDir * mockHitDistance);
				firstBounce.EndPower = 0.8f; // Drops slightly after hitting a wall

				path.Bounces.push_back(firstBounce);

				path.HitListener = false;

				scoutPaths.push_back(path);
			}


			OnScoutBatchReady.Broadcast(emitter.ID, std::move(scoutPaths));


			// TODO: In the future, you will run a loop here 9,900 times, bouncing rays.
			// Every time a ray hits the listener, you will calculate its total distance traveled
			// (which gives you the audio delay/reverb time) and its remaining power (volume), 
			// and mix it directly into the `finishedBake` buffer.

			/*
			for (int i = 0; i < HEAVY_RAY_COUNT; ++i)
			{
				// Shoot ray, bounce off walls, if it hits listener -> mix audio
			}
			*/

			CU::Vector3f dirToSource = sourcePos - listenerPos;
			float distance = dirToSource.Length();
			float minDistance = 100.0f;
			float maxDistance = 5000.0f;
			float rolloffFactor = 1.0f;
			float attenuation = 1.0f;

			if (distance > minDistance && distance < maxDistance)
			{
				attenuation = minDistance / (minDistance + rolloffFactor * (distance - minDistance));
			}
			else if (distance >= maxDistance)
			{
				attenuation = 0.0f;
			}

			dirToSource.Normalize();
			float pan = dirToSource.Dot(listenerRight);
			float pMapped = (pan + 1.0f) * 0.5f;
			const float PI = 3.14159265359f;
			float angle = pMapped * (PI * 0.5f);

			float leftGain = std::cos(angle) * attenuation;
			float rightGain = std::sin(angle) * attenuation;

			int numSamples = emitter.SourceBuffer->getNumSamples();
			const float* readPtr = emitter.SourceBuffer->getReadPointer(0);
			float* outLeft = finishedBake.getWritePointer(0);
			float* outRight = finishedBake.getWritePointer(1);

			for (int i = 0; i < numSamples; ++i)
			{
				outLeft[i] += readPtr[i] * leftGain;
				outRight[i] += readPtr[i] * rightGain;
			}
		}

		lock.lock();
		if (myHasWork == false)
		{
			OnBakeComplete.Broadcast(std::move(finishedBake));
		}
		lock.unlock();
	}
}
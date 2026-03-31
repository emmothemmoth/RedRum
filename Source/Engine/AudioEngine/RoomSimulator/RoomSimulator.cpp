#include "RoomSimulator.h"

#include <cmath>
#include <random>

#include "CommonUtilities/Intersection.hpp"

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
	std::lock_guard<std::mutex> lock(myMutex);

	AudioObstacle newObstacle;
	newObstacle.Transform = aTransform;

	// CRITICAL: Cache the inverse matrix here so the worker thread doesn't 
	// have to compute it 10,000 times a second!
	newObstacle.InverseTransform = aTransform.GetInverse();

	newObstacle.Absorber = someSettings;
	newObstacle.Collider = aCollider;
	newObstacle.ID = myObstacleIDCounter++;

	myObstacles.push_back(newObstacle);
	return newObstacle.ID;
}

void RoomSimulator::UpdateObstacle(const uint32_t aHandle, const AbsorberSettings& someSettings, const Collider& aCollider, const CU::Matrix4x4f& aTransform)
{
	std::lock_guard<std::mutex> lock(myMutex);

	for (auto& obstacle : myObstacles)
	{
		if (obstacle.ID == aHandle)
		{
			obstacle.Transform = aTransform;
			obstacle.InverseTransform = aTransform.GetInverse();
			obstacle.Absorber = someSettings;
			obstacle.Collider = aCollider;
			break; // Found it, stop searching
		}
	}
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
		// --- THE SLEEPING PHASE ---
		std::unique_lock<std::mutex> lock(myMutex);
		myConditionVariable.wait(lock, [this] { return myHasWork || myShouldExit; });

		if (myShouldExit) break;

		myHasWork = false;

		// --- 1. THREAD SAFETY: COPY THE DATA ---
		std::vector<AudioEmitter> localSources = mySources;
		std::vector<AudioObstacle> localObstacles = myObstacles;
		CU::Matrix4x4f localListener = myListener;
		lock.unlock();

		// --- 2. PREPARE THE OUTPUT BUFFER ---
		int maxSamples = 0;
		for (const auto& emitter : localSources)
		{
			if (emitter.SourceBuffer != nullptr)
			{
				int sampleCount = emitter.SourceBuffer->getNumSamples();
				maxSamples = sampleCount > maxSamples ? sampleCount : maxSamples;
			}
		}

		// Add extra space for the Reverb Tail/Delay (e.g., 2 seconds of extra audio)
		int sampleRate = 44100;
		int tailSamples = sampleRate * 2;
		juce::AudioBuffer<float> finishedBake(2, maxSamples + tailSamples);
		finishedBake.clear();

		// Extract Listener position and Right vector (Assuming 1-indexed matrices. Change to 0 if needed)
		CU::Vector3f listenerPos = { localListener(4,1), localListener(4,2), localListener(4,3) };
		CU::Vector3f listenerRight = { localListener(1,1), localListener(1,2), localListener(1,3) };
		listenerRight.Normalize();
		float listenerRadius = 50.0f; // 50cm capture radius

		// --- 3. THE RAYTRACING PROCESSING LOOP ---
		std::mt19937 rng(42);
		std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

		const int SCOUT_RAY_COUNT = 100;
		const int HEAVY_RAY_COUNT = 9900;
		const int TOTAL_RAYS = SCOUT_RAY_COUNT + HEAVY_RAY_COUNT;
		const int MAX_BOUNCES = 10;

		for (const auto& emitter : localSources)
		{
			if (!emitter.SourceBuffer || emitter.SourceBuffer->getNumSamples() == 0)
				continue;

			CU::Vector3f sourcePos = { emitter.Transform(4,1), emitter.Transform(4,2), emitter.Transform(4,3) };
			std::vector<VisualRayPath> scoutPaths;
			scoutPaths.reserve(SCOUT_RAY_COUNT);

			// THE MASTER RAY LOOP
			for (int rayIndex = 0; rayIndex < TOTAL_RAYS; ++rayIndex)
			{
				bool isScoutRay = (rayIndex < SCOUT_RAY_COUNT);

				CU::Vector3f currentRayPos = sourcePos;
				CU::Vector3f currentRayDir(dist(rng), dist(rng), dist(rng));
				currentRayDir.Normalize();

				float currentPower = 1.0f;
				float totalDistance = 0.0f;

				VisualRayPath currentPath;
				currentPath.HitListener = false;

				for (int bounce = 0; bounce < MAX_BOUNCES; ++bounce)
				{
					float closestT = INFINITY;
					CU::Vector3f bestNormal;
					bool hitListener = false;

					// A. CHECK LISTENER SPHERE INTERSECTION
					CU::Vector3f L = listenerPos - currentRayPos;
					float tca = L.Dot(currentRayDir);
					if (tca > 0.0f)
					{
						float d2 = L.Dot(L) - (tca * tca);
						float radius2 = listenerRadius * listenerRadius;
						if (d2 < radius2)
						{
							float thc = std::sqrt(radius2 - d2);
							float listenerT = tca - thc;
							if (listenerT > 0.01f && listenerT < closestT)
							{
								closestT = listenerT;
								hitListener = true;
							}
						}
					}

					// B. CHECK ALL WALLS/OBSTACLES
					for (const auto& obstacle : localObstacles)
					{
						// Transform ray to local space!
						CU::Vector4f localOrigin4 = obstacle.InverseTransform * CU::Vector4f(currentRayPos.x, currentRayPos.y, currentRayPos.z, 1.0f);
						CU::Vector4f localDir4 = obstacle.InverseTransform * CU::Vector4f(currentRayDir.x, currentRayDir.y, currentRayDir.z, 0.0f);
						CU::Vector3f localOrigin = { localOrigin4.x, localOrigin4.y, localOrigin4.z };
						CU::Vector3f localDir = { localDir4.x, localDir4.y, localDir4.z };

						if (std::holds_alternative<AABBCollider>(obstacle.Collider.Shape))
						{
							const auto& aabb = std::get<AABBCollider>(obstacle.Collider.Shape);

							float hitT;
							CU::Vector3f localNormal;
							if (CU::RaycastAABB(localOrigin, localDir, aabb.MinPoint, aabb.MaxPoint, hitT, localNormal))
							{
								// Scale T back to world space distance
								float worldT = hitT * localDir.Length();

								if (worldT > 0.01f && worldT < closestT)
								{
									closestT = worldT;
									hitListener = false; // Wall blocked the listener!

									// Transform normal back to world space
									CU::Vector4f worldNorm4 = obstacle.Transform * CU::Vector4f(localNormal.x, localNormal.y, localNormal.z, 0.0f);
									bestNormal = { worldNorm4.x, worldNorm4.y, worldNorm4.z };
									bestNormal.Normalize();
								}
							}
						}
					}

					// C. RESOLVE HIT
					if (closestT == INFINITY) break; // Ray flew out into the void

					CU::Vector3f hitPoint = currentRayPos + (currentRayDir * closestT);
					totalDistance += closestT;

					// Record Scout Data
					if (isScoutRay)
					{
						RayBounce visualBounce;
						visualBounce.StartPos = currentRayPos;
						visualBounce.EndPos = hitPoint;
						visualBounce.StartPower = currentPower;
						visualBounce.EndPower = hitListener ? currentPower : currentPower * 0.95f; // Simulate 30% absorption
						currentPath.Bounces.push_back(visualBounce);
					}

					if (hitListener)
					{
						if (isScoutRay) currentPath.HitListener = true;

						// --- AUDIO DSP MIXING ---
						// 1. Calculate Time Delay (Speed of sound ≈ 34300 cm/s)
						float delaySeconds = totalDistance / 34300.0f;
						int delaySamples = static_cast<int>(delaySeconds * sampleRate);

						// 2. Calculate Distance Attenuation (Inverse Square/Rolloff)
						float minDistance = 100.0f;
						float attenuation = minDistance / (minDistance + 1.0f * (totalDistance - minDistance));
						if (attenuation > 1.0f) attenuation = 1.0f;

						// Multiply by initial power and divide by total rays so it doesn't blow out your speakers!
						float finalGain = (attenuation * currentPower) / (TOTAL_RAYS * 0.1f);

						// 3. Panning (Based on the direction the ray hit the listener from)
						float pan = currentRayDir.Dot(listenerRight);
						float pMapped = (pan + 1.0f) * 0.5f;
						float angle = pMapped * (3.14159265359f * 0.5f);
						float leftGain = std::cos(angle) * finalGain;
						float rightGain = std::sin(angle) * finalGain;

						// 4. Mix into output buffer with delay
						int sourceLength = emitter.SourceBuffer->getNumSamples();
						const float* readPtr = emitter.SourceBuffer->getReadPointer(0);
						float* outLeft = finishedBake.getWritePointer(0);
						float* outRight = finishedBake.getWritePointer(1);

						for (int i = 0; i < sourceLength; ++i)
						{
							int writePos = i + delaySamples;
							if (writePos < finishedBake.getNumSamples())
							{
								outLeft[writePos] += readPtr[i] * leftGain;
								outRight[writePos] += readPtr[i] * rightGain;
							}
						}
						break; // Ray is consumed by the listener!
					}
					else
					{
						// D. REFLECT OFF WALL
						currentPower *= 0.95f; // Lose energy to the wall (TODO: Absorbersettings)

						float dotProduct = currentRayDir.Dot(bestNormal);
						currentRayDir = currentRayDir - (bestNormal * (2.0f * dotProduct));
						currentRayDir.Normalize();

						currentRayPos = hitPoint + (bestNormal * 0.1f); // Push slightly off the wall
					}
				}

				if (isScoutRay)
				{
					scoutPaths.push_back(currentPath);
				}
			}

			// Broadcast UI Rays
			OnScoutBatchReady.Broadcast(emitter.ID, std::move(scoutPaths));
		}

		// --- 4. THE CALLBACK ---
		lock.lock();
		if (myHasWork == false)
		{
			OnBakeComplete.Broadcast(std::move(finishedBake));
		}
		lock.unlock();
	}
}
#include "RoomSimulator.h"

#include <cmath>
#include <random>

#include "CommonUtilities/Intersection.hpp"
#include "../../GameEngine/MainSingleton.h"
#include "../../GraphicsEngine/Commands/GCmdComputeAcoustics.h"

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

void RoomSimulator::Update()
{
	// 1. Lock the handshake mutex (Super fast, won't stall the thread)
	std::lock_guard<std::mutex> computeLock(myComputeMutex);

	// 2. Check if the Worker Thread left a compute request for us
	if (myPendingComputeRequest.has_value())
	{
		// 3. Extract the request and clear the mailbox
		ComputeRequest req = std::move(myPendingComputeRequest.value());
		myPendingComputeRequest = std::nullopt;

		// 4. THIS IS THE MAGIC LINE: Enqueue the command to the Render Thread!
		MainSingleton::Get().GetRenderer().Enqueue<GCmdComputeAcoustics>(
			RenderStage::PreRendering, // Or whatever stage makes sense in your pipeline
			std::move(req.Rays),
			std::move(req.Obstacles),
			req.Promise
		);
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
		// ====================================================================
		// 1. THE SLEEPING & DATA GATHERING PHASE
		// ====================================================================
		std::unique_lock<std::mutex> lock(myMutex);
		myConditionVariable.wait(lock, [this] { return myHasWork || myShouldExit; });

		if (myShouldExit) break;
		myHasWork = false;

		// Safely copy the scene state so the Update Thread can keep moving
		std::vector<AudioEmitter> localSources = mySources;
		std::vector<AudioObstacle> localObstacles = myObstacles;
		CU::Matrix4x4f localListener = myListener;
		lock.unlock();

		// ====================================================================
		// 2. PREPARE THE AUDIO OUTPUT BUFFER
		// ====================================================================
		int maxSamples = 0;
		for (const auto& emitter : localSources)
		{
			if (emitter.SourceBuffer != nullptr)
			{
				int sampleCount = emitter.SourceBuffer->getNumSamples();
				maxSamples = sampleCount > maxSamples ? sampleCount : maxSamples;
			}
		}

		// Add 2 seconds of extra space for the Reverb Tail/Delay
		int sampleRate = 44100;
		int tailSamples = sampleRate * 2;
		juce::AudioBuffer<float> finishedBake(2, maxSamples + tailSamples);
		finishedBake.clear();

		// Extract Listener properties
		CU::Vector3f listenerPos = { localListener(4,1), localListener(4,2), localListener(4,3) };
		CU::Vector3f listenerRight = { localListener(1,1), localListener(1,2), localListener(1,3) };
		listenerRight.Normalize();
		float listenerRadius = 50.0f; // 50cm capture radius

		// Prepare RNG for ray directions
		std::mt19937 rng(42);
		std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

		const int SCOUT_RAY_COUNT = 100;
		const int HEAVY_RAY_COUNT = 9900;
		const int MAX_BOUNCES = 5;

		// ====================================================================
		// 3. PREPARE OBSTACLES FOR THE GPU
		// ====================================================================
		std::vector<GPUObstacle> gpuObstacles;
		gpuObstacles.reserve(localObstacles.size());

		for (const auto& obs : localObstacles)
		{
			if (std::holds_alternative<AABBCollider>(obs.Collider.Shape))
			{
				const auto& aabb = std::get<AABBCollider>(obs.Collider.Shape);

				GPUObstacle gpuObs;
				gpuObs.Transform = obs.Transform;
				gpuObs.InverseTransform = obs.InverseTransform;
				gpuObs.MinPoint = aabb.MinPoint;
				gpuObs.MaxPoint = aabb.MaxPoint;
				gpuObs.Absorption = obs.Absorber.ReflectionCoefficient; // e.g. 0.95f
				gpuObs.Padding = 0.0f;

				gpuObstacles.push_back(gpuObs);
			}
		}

		// ====================================================================
		// 4. THE PER-EMITTER PROCESSING LOOP
		// ====================================================================
		for (const auto& emitter : localSources)
		{
			if (!emitter.SourceBuffer || emitter.SourceBuffer->getNumSamples() == 0)
				continue;

			CU::Vector3f sourcePos = { emitter.Transform(4,1), emitter.Transform(4,2), emitter.Transform(4,3) };

			// ----------------------------------------------------------------
			// PHASE A: THE CPU SCOUT RAYS (For the Editor UI)
			// ----------------------------------------------------------------
			std::vector<VisualRayPath> scoutPaths;
			scoutPaths.reserve(SCOUT_RAY_COUNT);

			for (int i = 0; i < SCOUT_RAY_COUNT; ++i)
			{
				CU::Vector3f currentRayPos = sourcePos;
				CU::Vector3f currentRayDir(dist(rng), dist(rng), dist(rng));
				currentRayDir.Normalize();

				float currentPower = 1.0f;
				VisualRayPath currentPath;
				currentPath.HitListener = false;

				for (int bounce = 0; bounce < MAX_BOUNCES; ++bounce)
				{
					float closestT = INFINITY;
					CU::Vector3f bestNormal;
					bool hitListener = false;
					float hitAbsorption = 0.95f;

					// 1. Check Listener Sphere
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

					// 2. Check Obstacles
					for (const auto& obstacle : localObstacles)
					{
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
								float worldT = hitT * localDir.Length();
								if (worldT > 0.01f && worldT < closestT)
								{
									closestT = worldT;
									hitListener = false;
									hitAbsorption = obstacle.Absorber.ReflectionCoefficient;

									CU::Vector4f worldNorm4 = obstacle.Transform * CU::Vector4f(localNormal.x, localNormal.y, localNormal.z, 0.0f);
									bestNormal = { worldNorm4.x, worldNorm4.y, worldNorm4.z };
									bestNormal.Normalize();
								}
							}
						}
					}

					if (closestT == INFINITY) break;

					CU::Vector3f hitPoint = currentRayPos + (currentRayDir * closestT);

					// Record Scout Data
					RayBounce visualBounce;
					visualBounce.StartPos = currentRayPos;
					visualBounce.EndPos = hitPoint;
					visualBounce.StartPower = currentPower;
					visualBounce.EndPower = hitListener ? currentPower : currentPower * hitAbsorption;
					currentPath.Bounces.push_back(visualBounce);

					if (hitListener)
					{
						currentPath.HitListener = true;
						break;
					}
					else
					{
						currentPower *= hitAbsorption;
						float dotProduct = currentRayDir.Dot(bestNormal);
						currentRayDir = currentRayDir - (bestNormal * (2.0f * dotProduct));
						currentRayDir.Normalize();
						currentRayPos = hitPoint + (bestNormal * 0.1f);
					}
				}
				scoutPaths.push_back(currentPath);
			}

			// Broadcast UI Rays immediately so the Editor feels responsive
			OnScoutBatchReady.Broadcast(emitter.ID, std::move(scoutPaths));


			// ----------------------------------------------------------------
			// PHASE B: THE GPU HEAVY RAYS (For the Audio DSP)
			// ----------------------------------------------------------------
			std::vector<GPURay> gpuRays;
			gpuRays.reserve(HEAVY_RAY_COUNT);

			for (int i = 0; i < HEAVY_RAY_COUNT; ++i)
			{
				GPURay r;
				r.Origin = sourcePos;
				CU::Vector3f dir(dist(rng), dist(rng), dist(rng));
				dir.Normalize();
				r.Direction = dir;
				r.Power = 1.0f;
				r.Padding = 0.0f;
				gpuRays.push_back(r);
			}

			// 1. Create the Handshake
			auto promise = std::make_shared<std::promise<std::vector<GPURayResult>>>();
			std::future<std::vector<GPURayResult>> future = promise->get_future();

			// 2. Hand the request to the Update Thread safely
			{
				std::lock_guard<std::mutex> computeLock(myComputeMutex);
				ComputeRequest req;
				req.Rays = std::move(gpuRays);
				req.Obstacles = gpuObstacles; // Pass a copy of the prepared obstacles
				req.Promise = promise;

				myPendingComputeRequest = std::move(req);
			}

			// 3. 💤 SLEEP THE THREAD 💤 (Wait for the Render Thread)
			std::vector<GPURayResult> gpuResults = future.get();

			// 4. ☀️ WAKE UP! ☀️ (Mix the GPU results into the Audio Buffer)
			const float minDistance = 400.0f; // 4 meters
			const float rolloffFactor = 0.5f;
			const int sourceLength = emitter.SourceBuffer->getNumSamples();
			const float* readPtr = emitter.SourceBuffer->getReadPointer(0);
			float* outLeft = finishedBake.getWritePointer(0);
			float* outRight = finishedBake.getWritePointer(1);

			for (const auto& result : gpuResults)
			{
				if (result.HitListener == 1)
				{
					// Delay
					float delaySeconds = result.TotalDistance / 34300.0f;
					int delaySamples = static_cast<int>(delaySeconds * sampleRate);

					// Attenuation
					float attenuation = minDistance / (minDistance + rolloffFactor * (result.TotalDistance - minDistance));
					if (attenuation > 1.0f) attenuation = 1.0f;

					// Final Volume (Divide by heavy ray count to normalize the energy!)
					float finalGain = (attenuation * result.FinalPower) / (HEAVY_RAY_COUNT * 0.1f);

					// Panning (Already converted to 0 - PI/2 angle by the shader)
					float leftGain = std::cos(result.PanAngle) * finalGain;
					float rightGain = std::sin(result.PanAngle) * finalGain;

					// Additive Mix
					for (int i = 0; i < sourceLength; ++i)
					{
						int writePos = i + delaySamples;
						if (writePos < finishedBake.getNumSamples())
						{
							outLeft[writePos] += readPtr[i] * leftGain;
							outRight[writePos] += readPtr[i] * rightGain;
						}
					}
				}
			}
		}

		// ====================================================================
		// 5. THE CALLBACK
		// ====================================================================
		lock.lock();
		if (myHasWork == false)
		{
			OnBakeComplete.Broadcast(std::move(finishedBake));
		}
		lock.unlock();
	}
}
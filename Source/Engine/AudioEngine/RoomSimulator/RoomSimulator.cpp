#include "RoomSimulator.h"

#include <cmath>
#include <random>

#include "CommonUtilities/Intersection.hpp"
#include "../../GameEngine/MainSingleton.h"
#include "../../GraphicsEngine/Commands/GCmdComputeAcoustics.h"
#include "../../GraphicsEngine/Commands/GCmdComputeMegaAcoustics.h"

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
    std::lock_guard<std::mutex> computeLock(myComputeMutex);

    if (myPendingComputeRequest.has_value())
    {
        ComputeRequest req = std::move(myPendingComputeRequest.value());
        myPendingComputeRequest = std::nullopt;

        MainSingleton::Get().GetRenderer().Enqueue<GCmdComputeMegaAcoustics>(
            RenderStage::PreRendering,
            std::move(req.Rays),
            std::move(req.Obstacles),
            std::move(req.Probes),
            req.ProbeRadius,
            req.ListenerRight,
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

std::optional<uint32_t> RoomSimulator::RegisterEmitter(const juce::AudioBuffer<float>* aSourceBuffer, float aSampleRate, const EmitterSettings& someSettings, const CU::Matrix4x4f& aTransform)
{
	std::lock_guard<std::mutex> lock(myMutex);

	AudioEmitter newEmitter;
	newEmitter.SourceBuffer = aSourceBuffer;
	newEmitter.Settings = someSettings;
	newEmitter.Transform = aTransform;
	newEmitter.SampleRate = aSampleRate;
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
			break;
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

CU::Matrix4x4f RoomSimulator::GetListenerTransform()
{
    std::lock_guard<std::mutex> lock(myMutex);
    return myListener;
}

std::vector<AudioEmitter> RoomSimulator::GetSourcesCopy()
{
    std::lock_guard<std::mutex> lock(myMutex);
    return mySources;
}

std::vector<AudioObstacle> RoomSimulator::GetObstaclesCopy()
{
    std::lock_guard<std::mutex> lock(myMutex);
    return myObstacles;
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
        auto localSources = mySources;
        auto localObstacles = myObstacles;
        CU::Matrix4x4f localListener = myListener;
        lock.unlock();

        // Extract Listener properties for the Scout Rays and Panning
        CU::Vector3f listenerPos = { localListener(4,1), localListener(4,2), localListener(4,3) };
        CU::Vector3f listenerRight = { localListener(1,1), localListener(1,2), localListener(1,3) };
        listenerRight.Normalize();

        // Settings
        float listenerRadius = 100.0f;
        const int SCOUT_RAY_COUNT = 20;
        const int MAX_BOUNCES = 5;

        // Prepare RNG
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

        // ====================================================================
        // 2. SETUP THE PROBE GRID & OBSTACLES
        // ====================================================================
        CU::Vector3f minBounds = { -1000.0f, 0.0f, -1000.0f };
        CU::Vector3f maxBounds = { 1000.0f, 500.0f, 1000.0f };
        std::vector<CU::Vector3f> probePositions = GenerateGrid(minBounds, maxBounds, myProbeSpacing);

        std::vector<AcousticProbe> finalGrid;
        finalGrid.reserve(probePositions.size());
        for (auto& pos : probePositions)
        {
            finalGrid.push_back({ pos, {} });
        }

        std::vector<GPUObstacle> gpuObstacles;
        gpuObstacles.reserve(localObstacles.size());

        for (const auto& obs : localObstacles)
        {
            if (std::holds_alternative<AABBCollider>(obs.Collider.Shape))
            {
                const auto& aabb = std::get<AABBCollider>(obs.Collider.Shape);
                GPUObstacle gpuObs;
                gpuObs.InverseTransform = obs.InverseTransform;
                gpuObs.Transform = obs.Transform;
                gpuObs.MinPoint = aabb.MinPoint;
                gpuObs.Padding1 = 0.0f;
                gpuObs.MaxPoint = aabb.MaxPoint;
                gpuObs.Padding2 = 0.0f;
                gpuObs.Reflection = obs.Absorber.Reflection; // 3-Band Vector
                gpuObs.Padding3 = 0.0f;
                gpuObstacles.push_back(gpuObs);
            }
        }

        // ====================================================================
        // 3. THE PER-EMITTER PROCESSING LOOP
        // ====================================================================
        for (auto& emitter : localSources)
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

                // 3-Band Energy Tracker
                CU::Vector3f currentPower = { 1.0f, 1.0f, 1.0f };
                VisualRayPath currentPath;
                currentPath.HitListener = false;

                for (int bounce = 0; bounce < MAX_BOUNCES; ++bounce)
                {
                    float closestT = INFINITY;
                    CU::Vector3f bestNormal;
                    bool hitListener = false;
                    CU::Vector3f hitAbsorption = { 0.95f, 0.95f, 0.95f }; // Default wall

                    // 1. Check Listener Sphere (Editor Only)
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
                                    hitAbsorption = obstacle.Absorber.Reflection; // Save specific material hit

                                    CU::Vector4f worldNorm4 = obstacle.Transform * CU::Vector4f(localNormal.x, localNormal.y, localNormal.z, 0.0f);
                                    bestNormal = { worldNorm4.x, worldNorm4.y, worldNorm4.z };
                                    bestNormal.Normalize();
                                }
                            }
                        }
                    }

                    if (closestT == INFINITY) break;

                    CU::Vector3f hitPoint = currentRayPos + (currentRayDir * closestT);

                    RayBounce visualBounce;
                    visualBounce.StartPos = currentRayPos;
                    visualBounce.EndPos = hitPoint;

                    // Average the 3 bands to get a single alpha value for the debug line
                    visualBounce.StartPower = (currentPower.x + currentPower.y + currentPower.z) / 3.0f;

                    if (hitListener)
                    {
                        visualBounce.EndPower = visualBounce.StartPower;
                        currentPath.Bounces.push_back(visualBounce);
                        currentPath.HitListener = true;
                        break;
                    }
                    else
                    {
                        // Calculate UI End Power
                        float endAvg = ((currentPower.x * hitAbsorption.x) +
                            (currentPower.y * hitAbsorption.y) +
                            (currentPower.z * hitAbsorption.z)) / 3.0f;

                        visualBounce.EndPower = endAvg;
                        currentPath.Bounces.push_back(visualBounce);

                        // Apply full 3-Band Absorption
                        currentPower.x *= hitAbsorption.x;
                        currentPower.y *= hitAbsorption.y;
                        currentPower.z *= hitAbsorption.z;

                        float dotProduct = currentRayDir.Dot(bestNormal);
                        currentRayDir = currentRayDir - (bestNormal * (2.0f * dotProduct));
                        currentRayDir.Normalize();
                        currentRayPos = hitPoint + (bestNormal * 0.1f);
                    }
                }
                scoutPaths.push_back(currentPath);
            }

            // Broadcast UI Rays immediately
            OnScoutBatchReady.Broadcast(emitter.ID, std::move(scoutPaths));


            // ----------------------------------------------------------------
            // PHASE B: THE GPU MEGA BAKE
            // ----------------------------------------------------------------
            std::vector<GPURay> gpuRays;
            gpuRays.reserve(myRayLimit);

            for (int i = 0; i < myRayLimit; ++i)
            {
                GPURay r;
                r.Origin = sourcePos;
                r.Padding1 = 0.0f;
                r.Power = CU::Vector3f(1.0f, 1.0f, 1.0f); // 3-Band Energy starts at 100%
                r.Padding2 = 0.0f;
                r.Direction = CU::Vector3f(dist(rng), dist(rng), dist(rng)).GetNormalized();
                r.Padding3 = 0.0f;
                gpuRays.push_back(r);
            }

            auto promise = std::make_shared<std::promise<std::vector<GPUMegaHit>>>();
            auto future = promise->get_future();

            // --- THE SAFE HAND-OFF ---
            {
                std::lock_guard<std::mutex> computeLock(myComputeMutex);
                ComputeRequest req;
                req.Rays = std::move(gpuRays);
                req.Obstacles = gpuObstacles; // Copy
                req.Probes = probePositions;  // Copy
                req.ProbeRadius = listenerRadius;
                req.ListenerRight = listenerRight;
                req.Promise = promise;

                myPendingComputeRequest = std::move(req);
            }

            std::vector<GPUMegaHit> allHits;
            try
            {
                allHits = future.get();
            }
            catch (const std::future_error& e)
            {
                e;
                // If the GPU queue dropped the command, gracefully skip this bake
                // and keep the thread alive!
                continue;
            }

            // Sort the hits into their respective probes
            for (const auto& hit : allHits)
            {
                if (hit.ProbeIndex >= 0 && hit.ProbeIndex < finalGrid.size())
                {
                    finalGrid[hit.ProbeIndex].Hits.push_back(hit);
                }
            }
        }

        // ====================================================================
        // 4. THE CALLBACK
        // ====================================================================
        // We broadcast the entire grid. The Audio Engine will handle mixing.
        lock.lock();
        if (myHasWork == false)
        {
            OnMegaBakeComplete.Broadcast(std::move(finalGrid));
        }
        lock.unlock();
    }
}

std::vector<CU::Vector3f> RoomSimulator::GenerateGrid(CU::Vector3f minBounds, CU::Vector3f maxBounds, float spacing)
{
	std::vector<CU::Vector3f> positions;
	for (float x = minBounds.x; x <= maxBounds.x; x += spacing)
	{
		for (float y = minBounds.y; y <= maxBounds.y; y += spacing)
		{
			for (float z = minBounds.z; z <= maxBounds.z; z += spacing)
			{
				positions.push_back({ x, y, z });
			}
		}
	}
	return positions;
}

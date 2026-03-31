#include "RoomSimulator.h"

#include <cmath>

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
        // --- THE SLEEPING PHASE ---
        std::unique_lock<std::mutex> lock(myMutex);

        myConditionVariable.wait(lock, [this] { return myHasWork || myShouldExit; });

        if (myShouldExit) break;

        myHasWork = false;

        // --- 1. THREAD SAFETY: COPY THE DATA ---
        std::vector<AudioEmitter> localSources = mySources;
        CU::Matrix4x4f localListener = myListener;

        // Unlock so the editor can keep running smoothly while we crunch numbers!
        lock.unlock();

        // --- 2. PREPARE THE OUTPUT BUFFER ---
        // Find the longest audio source so we know how big to make our wet buffer
        int maxSamples = 0;
        for (const auto& emitter : localSources)
        {
            if (emitter.SourceBuffer != nullptr)
            {
                maxSamples = std::max(maxSamples, emitter.SourceBuffer->getNumSamples());
            }
        }

        juce::AudioBuffer<float> finishedBake(2, maxSamples);
        finishedBake.clear(); // Ensure it's filled with silence initially

        // Extract Listener position and Right vector
        // NOTE: Adjust the indices (4,1 etc) based on how your Matrix4x4 is structured!
        CU::Vector3f listenerPos = { localListener(4,1), localListener(4,2), localListener(4,3) };
        CU::Vector3f listenerRight = { localListener(1,1), localListener(1,2), localListener(1,3) };
        listenerRight.Normalize();

        // --- 3. THE DSP PROCESSING LOOP ---
        for (const auto& emitter : localSources)
        {
            if (!emitter.SourceBuffer || emitter.SourceBuffer->getNumSamples() == 0)
                continue;

            // Calculate Distance and Direction
            CU::Vector3f sourcePos = { emitter.Transform(4,1), emitter.Transform(4,2), emitter.Transform(4,3) };
            CU::Vector3f dirToSource = sourcePos - listenerPos;

            float distance = dirToSource.Length();
            float attenuation = 1.0f;

            // Using sensible defaults if they aren't in your EmitterSettings yet
            float minDistance = 100.0f;  // 1 meter (100 cm)
            float maxDistance = 5000.0f; // 50 meters (5000 cm)
            float rolloffFactor = 1.0f;

            if (distance <= minDistance)
            {
                attenuation = 1.0f; // Inside the "full volume" bubble
            }
            else if (distance >= maxDistance)
            {
                attenuation = 0.0f; // Outside the hearing range, hard cutoff
            }
            else
            {
                // The Industry Standard Inverse Distance Curve
                attenuation = minDistance / (minDistance + rolloffFactor * (distance - minDistance));

                // Optional: Many game engines do a secondary calculation here to ensure 
                // the curve cleanly hits exactly 0.0 at the MaxDistance to prevent audio pops.
                // For pure simulation, the raw curve is often fine.
            }

            // Constant Power Panning
            dirToSource.Normalize();
            float pan = dirToSource.Dot(listenerRight); // Range: -1 (Left) to 1 (Right)

            float pMapped = (pan + 1.0f) * 0.5f;
            const float PI = 3.14159265359f;
            float angle = pMapped * (PI * 0.5f);

            float leftGain = std::cos(angle) * attenuation;
            float rightGain = std::sin(angle) * attenuation;

            // Mix the emitter into the final buffer
            int numSamples = emitter.SourceBuffer->getNumSamples();

            // Assuming the source file is Mono for 3D spatialization. 
            // If it's stereo, you'd usually mix it down to mono first, or process channels separately.
            const float* readPtr = emitter.SourceBuffer->getReadPointer(0);

            float* outLeft = finishedBake.getWritePointer(0);
            float* outRight = finishedBake.getWritePointer(1);

            for (int i = 0; i < numSamples; ++i)
            {
                outLeft[i] += readPtr[i] * leftGain;
                outRight[i] += readPtr[i] * rightGain;
            }
        }

        // --- 4. THE CALLBACK ---
        // Let the AudioEngine know we are done!
        // We do a quick lock check to ensure we didn't get a new Bake request while calculating
        lock.lock();
        if (myHasWork == false)
        {
            OnBakeComplete.Broadcast(std::move(finishedBake));
        }
        lock.unlock();
    }
}

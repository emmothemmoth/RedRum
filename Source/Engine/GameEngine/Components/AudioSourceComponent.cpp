#include "GameEngine.pch.h"
#include "AudioSourceComponent.h"

#include "GameObject.h"
#include "CommonUtilities/Input.h"
#include <random>
#include "../../GraphicsEngine/Commands/GCmdDrawDebugLines.h"

AudioSourceComponent::AudioSourceComponent(GameObject& aParent)
	: Component(aParent)
{
	myComponentType = ComponentType::AudioSource;
	myRenderStages.at(RenderStage::ShadowMapping) = false;
	myRenderStages.at(RenderStage::Deferred) = false;

	auto& audioEngine = MainSingleton::Get().GetAudioEngine();
	myVisualizeHandle = audioEngine.OnSimulationStarted.AddRaw(this, &AudioSourceComponent::StartVisualRays);
}

AudioSourceComponent::~AudioSourceComponent()
{
	auto& audioEngine = MainSingleton::Get().GetAudioEngine();
	audioEngine.UnregisterEmitter(myEmitterHandle);

	audioEngine.OnSimulationStarted.Remove(myVisualizeHandle);
}

void AudioSourceComponent::Init(const std::filesystem::path& aSourceFile)
{
	AudioEngine& engine = MainSingleton::Get().GetAudioEngine();
	auto handle = engine.RegisterSoundSource(aSourceFile);
	if (handle)
	{
		mySourceHandle = handle.value();
		myIsPlayable = true;
		auto emitterHandle = engine.RegisterAudioEmitter(mySourceHandle, mySettings, myParent.GetTransform());
		if (emitterHandle)
		{
			myEmitterHandle = emitterHandle.value();
		}
	}

}

void AudioSourceComponent::Update(const float aDeltaTime)
{
	aDeltaTime;
	if (myParent.IsDirty() || myIsDirty)
	{
		myIsDirty = false;
		MainSingleton::Get().GetAudioEngine().UpdateAudioEmitter(myEmitterHandle, mySettings, myParent.GetTransform());
	}
	if (myIsAnimatingRays)
	{
		bool allFinished = true;

		for (auto& ray : myVisualRays)
		{
			if (!ray.IsFinished)
			{
				ray.CurrentDistance += ray.Speed * aDeltaTime;

				if (ray.CurrentDistance >= ray.MaxDistance)
				{
					ray.CurrentDistance = ray.MaxDistance;
					ray.IsFinished = true;
				}
				else
				{
					allFinished = false;
				}
			}
		}

		// Optional: Stop drawing them a few seconds after they hit the walls
		if (allFinished)
		{
			// myIsAnimatingRays = false; 
		}
	}
}

void AudioSourceComponent::Render()
{
	if (!myIsVisible) return;
	if (!myIsAnimatingRays) return;
	if (myVisualRays.empty()) return;

	// 1. Clear CPU vectors (keeps GPU capacity intact)
	myDebugLines->ResetLines();

	// 2. Build the new frame's lines
	for (const auto& ray : myVisualRays)
	{
		CU::Vector3f endPos = ray.Origin + (ray.Direction * ray.CurrentDistance);
		myDebugLines->AddLine(ray.Origin, endPos);
	}

	// 3. Set the color for the newly generated vertices
	myDebugLines->SetColor(DebugColor::Pink);

	// 4. STREAM TO GPU (Replaces Initialize!)
	// This uses the lock-free mapping we just wrote, allowing it to run at thousands of FPS
	//myDebugLines->UpdateBuffers();

	// 5. Submit to the Render Queue
	MainSingleton::Get().GetRenderer().Enqueue<GCmdDrawDebugLines>(
		RenderStage::Forward, // Or whatever stage you draw debug lines
		myDebugLines,
		CU::Matrix4x4f() // Identity matrix, because our lines are already in world space!
	);
}

void AudioSourceComponent::Play()
{
	if (!myIsPlayable) return;
	AudioEngine& engine = MainSingleton::Get().GetAudioEngine();
	engine.Control2DSource(mySourceHandle, AudiosourceControl::Play);
}

void AudioSourceComponent::Pause()
{
	AudioEngine& engine = MainSingleton::Get().GetAudioEngine();
	engine.Control2DSource(mySourceHandle, AudiosourceControl::Pause);
}

void AudioSourceComponent::Stop()
{
	AudioEngine& engine = MainSingleton::Get().GetAudioEngine();
	engine.Control2DSource(mySourceHandle, AudiosourceControl::Stop);
}

void AudioSourceComponent::StartVisualRays()
{
	myVisualRays.clear();
	myIsAnimatingRays = true;

	if (!myDebugLines)
	{
		myDebugLines = std::make_shared<DebugLineObject>();
		myDebugLines->SetName("Simulation_Rays");
		myDebugLines->SetTopology(2);

		// PRE-ALLOCATE THE GPU MEMORY (100 rays * 2 vertices/indices)
		myDebugLines->Reserve(200, 200);
	}

	CU::Vector3f startPos = myParent.GetPosition();

	// Generate 100 random rays in a sphere
	std::mt19937 rng(42); // Seeded so the visual burst looks the same every time
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

	for (int i = 0; i < 100; ++i)
	{
		CU::Vector3f randomDir(dist(rng), dist(rng), dist(rng));
		randomDir.Normalize();

		VisualRay ray;
		ray.Origin = startPos;
		ray.Direction = randomDir;
		ray.Speed = 800.0f + (dist(rng) * 200.0f); // Slight speed variation looks cooler

		// TODO: Physics Raycast here to find the actual distance to the nearest wall!
		// float hitDistance = PhysicsEngine::Raycast(startPos, randomDir);
		// ray.MaxDistance = hitDistance; 

		ray.CurrentDistance = 0.0f;
		myVisualRays.push_back(ray);
	}
}

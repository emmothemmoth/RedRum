#include "GameEngine.pch.h"
#include "AudioSourceComponent.h"

#include "GameObject.h"
#include "CommonUtilities/Input.h"
#include <random>
#include "../../GraphicsEngine/Commands/GCmdDrawDebugLines.h"
#include "../../GraphicsEngine/Objects/DebugLineObject.h"

AudioSourceComponent::AudioSourceComponent(GameObject& aParent)
	: Component(aParent)
{
	myComponentType = ComponentType::AudioSource;
	myRenderStages.at(RenderStage::ShadowMapping) = false;
	myRenderStages.at(RenderStage::Deferred) = false;

	auto& audioEngine = MainSingleton::Get().GetAudioEngine();
	myVisualizeHandle = audioEngine.OnSimulationStarted.AddRaw(this, &AudioSourceComponent::ClearRays);
	myScoutHandle = audioEngine.OnVisualRaysReady.AddRaw(this, &AudioSourceComponent::ReceiveScoutRays);
}

AudioSourceComponent::~AudioSourceComponent()
{
	auto& audioEngine = MainSingleton::Get().GetAudioEngine();
	audioEngine.UnregisterEmitter(myEmitterHandle);

	audioEngine.OnSimulationStarted.Remove(myVisualizeHandle);
	audioEngine.OnVisualRaysReady.Remove(myScoutHandle);
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
	if (myParent.IsDirty() || myIsDirty)
	{
		myIsDirty = false;
		MainSingleton::Get().GetAudioEngine().UpdateAudioEmitter(myEmitterHandle, mySettings, myParent.GetTransform());
	}

	if (myIsAnimatingRays)
	{
		bool allFinished = true;

		for (auto& anim : myAnimators)
		{
			if (anim.CurrentBounceIndex >= anim.PathData.Bounces.size())
				continue;

			allFinished = false;

			const auto& bounce = anim.PathData.Bounces[anim.CurrentBounceIndex];

			CU::Vector3f dir = bounce.EndPos - bounce.StartPos;
			float segmentLength = dir.Length();

			if (segmentLength > 0.0001f) 
			{
				float distanceToMove = anim.Speed * aDeltaTime;
				anim.CurrentBounceProgress += (distanceToMove / segmentLength);
			}
			else
			{
				anim.CurrentBounceProgress = 1.0f;
			}

			if (anim.CurrentBounceProgress >= 1.0f)
			{
				anim.CurrentBounceIndex++;
				anim.CurrentBounceProgress = 0.0f;
			}
		}

		if (allFinished)
		{
			// myIsAnimatingRays = false; 
		}
	}
}

void AudioSourceComponent::Render()
{
	if (!myIsVisible) return;
	if (!myIsAnimatingRays || myAnimators.empty()) return;

	myDebugLines->ResetLines();
	myDebugLines->SetTopology(2);

	for (const auto& anim : myAnimators)
	{
		CU::Vector4f baseColor = anim.PathData.HitListener ? CU::Vector4f(0.0f, 1.0f, 0.0f, 1.0f) : CU::Vector4f( 1.0f, 0.0f, 1.0f, 1.0f);

		for (int i = 0; i < anim.CurrentBounceIndex; ++i)
		{
			const auto& bounce = anim.PathData.Bounces[i];
			CU::Vector4f startColor = baseColor; startColor.w = bounce.StartPower;
			CU::Vector4f endColor = baseColor;   endColor.w = bounce.EndPower;

			myDebugLines->AddLine(bounce.StartPos, bounce.EndPos, startColor, endColor);
		}

		if (anim.CurrentBounceIndex < anim.PathData.Bounces.size())
		{
			const auto& bounce = anim.PathData.Bounces[anim.CurrentBounceIndex];

			CU::Vector3f currentEndPos = bounce.StartPos + ((bounce.EndPos - bounce.StartPos) * anim.CurrentBounceProgress);
			float currentPower = bounce.StartPower + ((bounce.EndPower - bounce.StartPower) * anim.CurrentBounceProgress);

			CU::Vector4f startColor = baseColor; startColor.w = bounce.StartPower;
			CU::Vector4f endColor = baseColor;   endColor.w = currentPower;

			myDebugLines->AddLine(bounce.StartPos, currentEndPos, startColor, endColor);
		}
	}

	MainSingleton::Get().GetRenderer().Enqueue<GCmdDrawDebugLines>(
		RenderStage::Forward,
		myDebugLines,
		CU::Matrix4x4f()
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

void AudioSourceComponent::ClearRays()
{
	myAnimators.clear();
	myIsAnimatingRays = false;

	if (myDebugLines)
	{
		myDebugLines->ResetLines();
		myDebugLines->UpdateBuffers();
	}
}

void AudioSourceComponent::ReceiveScoutRays(EmitterHandle aHandle, std::vector<VisualRayPath> somePaths)
{
	if (aHandle != myEmitterHandle) return;

	myAnimators.clear();
	int totalLineSegments = 0;

	for (const auto& path : somePaths)
	{
		RayAnimationTracker anim;
		anim.PathData = path;
		anim.Speed = 500.0f;
		myAnimators.push_back(anim);

		totalLineSegments += static_cast<int>(path.Bounces.size());
	}

	if (!myDebugLines)
	{
		myDebugLines = std::make_shared<DebugLineObject>();
		myDebugLines->SetName("Simulation_Rays");
	}
	myDebugLines->Reserve(totalLineSegments * 2, totalLineSegments * 2);

	myIsAnimatingRays = true;
}

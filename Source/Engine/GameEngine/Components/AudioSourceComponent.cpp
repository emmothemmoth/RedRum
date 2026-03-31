#include "GameEngine.pch.h"
#include "AudioSourceComponent.h"

#include "GameObject.h"
#include "MainSingleton.h"
#include "CommonUtilities/Input.h"

AudioSourceComponent::AudioSourceComponent(GameObject& aParent)
	: Component(aParent)
{
	myComponentType = ComponentType::AudioSource;
	myRenderStages.at(RenderStage::ShadowMapping) = false;
	myRenderStages.at(RenderStage::Deferred) = false;
}

AudioSourceComponent::~AudioSourceComponent()
{
	//Unregister audio source
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
}

void AudioSourceComponent::Render()
{

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

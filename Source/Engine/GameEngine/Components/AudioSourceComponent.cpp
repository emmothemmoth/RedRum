#include "GameEngine.pch.h"
#include "AudioSourceComponent.h"

#include "GameObject.h"
#include "MainSingleton.h"

AudioSourceComponent::AudioSourceComponent(GameObject& aParent)
	: Component(aParent)
{
	//Register audio source
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
		myAudioHandle = handle.value();
		myIsPlayable = true;
	}

}

void AudioSourceComponent::ManualUpdate()
{
	AudioEngine& engine = MainSingleton::Get().GetAudioEngine();
	engine.UpdateSoundSource(myAudioHandle, myParent.GetTransform());
}

void AudioSourceComponent::Play()
{
	//Play the original file
}

void AudioSourceComponent::Stop()
{
	//Stop the original file
}

#include "GameEngine.pch.h"
#include "AudioListenerComponent.h"

AudioListenerComponent::AudioListenerComponent(GameObject& aParent)
	: Component(aParent)
{
}

AudioListenerComponent::~AudioListenerComponent()
{
}

void AudioListenerComponent::StartPreview()
{
	assert(myIsPlayable == true);
}

void AudioListenerComponent::StopPreview()
{
}

void AudioListenerComponent::ManualUpdate()
{
	//Update settings in audio engine: position, rotation, master volume
}

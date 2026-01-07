#include "GameEngine.pch.h"
#include "AudioSourceComponent.h"

AudioSourceComponent::AudioSourceComponent(GameObject& aParent)
	: Component(aParent)
{
	//Register audio source
}

AudioSourceComponent::~AudioSourceComponent()
{
	//Unregister audio source
}

void AudioSourceComponent::Play()
{
	//Play the original file
}

void AudioSourceComponent::Stop()
{
	//Stop the original file
}

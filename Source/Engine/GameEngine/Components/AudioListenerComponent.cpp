#include "GameEngine.pch.h"
#include "AudioListenerComponent.h"

AudioListenerComponent::AudioListenerComponent(GameObject& aParent)
	: Component(aParent)
{
	myRenderStages.at(RenderStage::ShadowMapping) = false;
	myRenderStages.at(RenderStage::Deferred) = false;
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

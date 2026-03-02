#include "GameEngine.pch.h"
#include "AudioSourceComponent.h"

#include "GameObject.h"
#include "MainSingleton.h"
#include "CommonUtilities/Input.h"

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

//TEMP
void AudioSourceComponent::Update(const float aDeltaTime)
{
	aDeltaTime;
	if (CU::Input::GetKeyUp(CU::Keys::SPACE))
	{
		Play();
	}
	if (CU::Input::GetKeyUp(CU::Keys::RETURN))
	{
		Stop();
	}
}

void AudioSourceComponent::Render()
{
	//Draw icon
}

void AudioSourceComponent::Play()
{
	if (!myIsPlayable) return;
	AudioEngine& engine = MainSingleton::Get().GetAudioEngine();
	engine.Control2DSource(myAudioHandle, AudiosourceControl::Play);
}

void AudioSourceComponent::Pause()
{
	AudioEngine& engine = MainSingleton::Get().GetAudioEngine();
	engine.Control2DSource(myAudioHandle, AudiosourceControl::Pause);
}

void AudioSourceComponent::Stop()
{
	AudioEngine& engine = MainSingleton::Get().GetAudioEngine();
	engine.Control2DSource(myAudioHandle, AudiosourceControl::Stop);
}

void AudioSourceComponent::OnEditorChange()
{
	AudioEngine& engine = MainSingleton::Get().GetAudioEngine();
	engine.UpdateSoundSource(myAudioHandle, myParent.GetTransform());
}

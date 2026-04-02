#include "GameEngine.pch.h"
#include "ListenerComponent.h"
#include "GameObject.h"
#include "MainSingleton.h"

ListenerComponent::ListenerComponent(GameObject& aParent)
	: Component(aParent)
{
	myRenderStages.at(RenderStage::ShadowMapping) = false;
	myRenderStages.at(RenderStage::Deferred) = false;
	auto& audioEngine = MainSingleton::Get().GetAudioEngine();
	myReadyHandle = audioEngine.OnSimulationReady.AddRaw(this, &ListenerComponent::OnSimulationFinished);
	myStartedHandle = audioEngine.OnSimulationStarted.AddRaw(this, &ListenerComponent::OnSimulationBegin);
	audioEngine.InitListener(myParent.GetTransform());
}

ListenerComponent::~ListenerComponent()
{
	auto& audioEngine = MainSingleton::Get().GetAudioEngine();

	// Safely remove ourselves from the engine's broadcast list before we die
	audioEngine.OnSimulationReady.Remove(myReadyHandle);
	audioEngine.OnSimulationStarted.Remove(myStartedHandle);
}

void ListenerComponent::Update(const float aDeltaTime)
{
	aDeltaTime;
	if (myParent.IsDirty())
	{
		MainSingleton::Get().GetAudioEngine().UpdateListener(myParent.GetTransform());
	}
}

void ListenerComponent::StartPreview()
{
	auto& audioEngine = MainSingleton::Get().GetAudioEngine();
	audioEngine.ControlRoomPlayback(AudiosourceControl::Play);
}

void ListenerComponent::StopPreview()
{
	auto& audioEngine = MainSingleton::Get().GetAudioEngine();
	audioEngine.ControlRoomPlayback(AudiosourceControl::Stop);
}

bool ListenerComponent::IsPlayable() const
{
	bool canPlay = myIsPlayable.load(std::memory_order_acquire);
	return canPlay;
}

void ListenerComponent::OnSimulationBegin()
{
	myIsPlayable.store(false, std::memory_order_release);

	if (mySimulationHandle != UINT32_MAX)
	{
		StopPreview();
	}
}

void ListenerComponent::OnSimulationFinished(AudioHandle aHandle)
{
	mySimulationHandle = aHandle;

	myIsPlayable.store(true, std::memory_order_release);
}

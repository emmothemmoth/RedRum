#pragma once
#include "Component.h"

#include "../AudioEngine/AudioEngine.h"

#include <atomic>

class ListenerComponent : public Component
{
public:
	ListenerComponent(GameObject& aParent);
	~ListenerComponent();

	void Update(const float aDeltaTime) override;

	//Play the simulation file
	void StartPreview();
	//Stop the simulation file
	void StopPreview();

	//If the room simulation is ready for playback
	bool IsPlayable() const;

	void OnSimulationBegin();
	void OnSimulationFinished(AudioHandle aHandle);

private:
	std::atomic<bool> myIsPlayable = false;
	AudioHandle mySimulationHandle = 0;
	DelegateHandle myReadyHandle;
	DelegateHandle myStartedHandle;
};


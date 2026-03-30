#pragma once
#include "Component.h"

class ListenerComponent : public Component
{
public:
	ListenerComponent(GameObject& aParent);
	~ListenerComponent();

	//Play the simulation file
	void StartPreview();
	//Stop the simulation file
	void StopPreview();

	//If the room simulation is ready for playback
	bool IsPlayable() const { return myIsPlayable; }

private:
	bool myIsPlayable = false;
};


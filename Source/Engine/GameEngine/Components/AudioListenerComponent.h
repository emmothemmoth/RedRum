#pragma once
#include "Component.h"

class AudioListenerComponent : public Component
{
public:
	AudioListenerComponent(GameObject& aParent);
	~AudioListenerComponent();

	void StartPreview();
	void StopPreview();

	//Updates the settings for listener
	void ManualUpdate();

	bool IsPlayable() const { return myIsPlayable; }

private:
	bool myIsPlayable = false;
};


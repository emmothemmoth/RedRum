#pragma once
#include "Component.h"

class ListenerComponent : public Component
{
public:
	ListenerComponent(GameObject& aParent);
	~ListenerComponent();

	void StartPreview();
	void StopPreview();

	//Updates the settings for listener
	void ManualUpdate();


	bool IsPlayable() const { return myIsPlayable; }

private:
	bool myIsPlayable = false;
};


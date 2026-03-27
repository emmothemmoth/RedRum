#include "GameEngine.pch.h"
#include "ListenerComponent.h"

ListenerComponent::ListenerComponent(GameObject& aParent)
	: Component(aParent)
{
	myRenderStages.at(RenderStage::ShadowMapping) = false;
	myRenderStages.at(RenderStage::Deferred) = false;
}

ListenerComponent::~ListenerComponent()
{
}

void ListenerComponent::StartPreview()
{
	assert(myIsPlayable == true);
}

void ListenerComponent::StopPreview()
{
}

void ListenerComponent::ManualUpdate()
{
	//Update settings in audio engine: position, rotation, master volume
}

#include "GameEngine.pch.h"
#include "Component.h"

#include "GameObject.h"

Component::Component(GameObject& aParent) : myParent(aParent)
,myComponentType(ComponentType::Count)
{
	myRenderStages.insert({ RenderStage::ShadowMapping, false });
	myRenderStages.insert({ RenderStage::Deferred, true });
	myRenderStages.insert({ RenderStage::Forward, false });
	myRenderStages.insert({ RenderStage::ObjectIDRendering, false });
	myRenderStages.insert({ RenderStage::Particles, false });
	myRenderStages.insert({ RenderStage::Custom, false });
	myRenderStages.insert({ RenderStage::WorldSpaceUI, false });
	myRenderStages.insert({ RenderStage::PostProcess, false });
	myRenderStages.insert({ RenderStage::Sprite, false });
}

Component::~Component()
{
}

void Component::Update(const float aDeltaTime)
{
	aDeltaTime;
}





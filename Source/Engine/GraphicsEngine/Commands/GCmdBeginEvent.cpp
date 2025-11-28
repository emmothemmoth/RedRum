#include "GraphicsEngine.pch.h"
#include "GCmdBeginEvent.h"

#include "GraphicsEngine.h"
#include "../GameEngine/MainSingleton.h"

GCmdBeginEvent::GCmdBeginEvent(std::string_view aName, RenderQueueStage aRenderQueueStage)
{
	int stage = static_cast<int>(aRenderQueueStage);
	assert(stage >= 0 && stage < static_cast<int>(RenderQueueStage::Count));
	stage;
	auto& renderer = RENDERER;
	renderer.ChangeRenderPass(aRenderQueueStage);
	myName = aName;
}

void GCmdBeginEvent::Execute()
{
	GraphicsEngine::Get().BeginEvent(myName);
}

void GCmdBeginEvent::Destroy()
{
	myName.~basic_string_view();
}

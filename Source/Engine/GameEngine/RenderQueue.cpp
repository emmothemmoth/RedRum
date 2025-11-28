#include "GameEngine.pch.h"
#include "RenderQueue.h"

#include "../GraphicsEngine/GraphicsEngine.h"
#include "../GraphicsEngine/Commands/GCmdChangePipelineState.h"

#include "../GraphicsEngine/PipelineStates.h"

#include <iostream>

RenderQueue::RenderQueue()
{
}

void RenderQueue::RenderFrame()
{
	//PrintDebugInfo();
	if (myShadowList.HasCommands())
	{
		myShadowList.Execute();
	}
	myShadowList.Reset();

	if (myDeferredList.HasCommands())
	{
		myDeferredList.Execute();
	}
	myDeferredList.Reset();

	if (myForwardList.HasCommands())
	{
		myForwardList.Execute();
	}
	myForwardList.Reset();

	if (myParticleList.HasCommands())
	{
		myParticleList.Execute();
	}
	myParticleList.Reset();

	if (myDebugList.HasCommands())
	{
		myDebugList.Execute();
	}
	myDebugList.Reset();

	if (myCustomList.HasCommands())
	{
		myCustomList.Execute();
	}
	myCustomList.Reset();

	if (myPostProcessList.HasCommands())
	{
		myPostProcessList.Execute();
	}
	myPostProcessList.Reset();

	if (mySpriteList.HasCommands())
	{
		mySpriteList.Execute();
	}
	mySpriteList.Reset();
}

void RenderQueue::Reset(bool aClearLists)
{
	if (aClearLists)
	{
		myShadowList.Reset();
		myDeferredList.Reset();
		myForwardList.Reset();
		myParticleList.Reset();
		myDebugList.Reset();
		myCustomList.Reset();
		myPostProcessList.Reset();
		mySpriteList.Reset();
	}

	myShadowList.Enqueue<GCmdChangePipelineState>(static_cast<unsigned>(PipelineStates::DirlightShadowMapping));
	myDeferredList.Enqueue<GCmdChangePipelineState>(static_cast<unsigned>(PipelineStates::DeferredRendering));
	myForwardList.Enqueue<GCmdChangePipelineState>(static_cast<unsigned>(PipelineStates::ForwardRendering));
	myParticleList.Enqueue<GCmdChangePipelineState>(static_cast<unsigned>(PipelineStates::ParticleRendering));
	myCustomList.Enqueue<GCmdChangePipelineState>(static_cast<unsigned>(PipelineStates::CustomRendering));
	myPostProcessList.Enqueue<GCmdChangePipelineState>(static_cast<unsigned>(PipelineStates::CustomPostProcess));
	mySpriteList.Enqueue<GCmdChangePipelineState>(static_cast<unsigned>(PipelineStates::SpriteRendering));
}

void RenderQueue::PrintDebugInfo()
{
	std::cout << "Shadow commands size: " << myShadowList.GetSize() << std::endl;
	std::cout << "Deferred commands size: " << myDeferredList.GetSize() << std::endl;
	std::cout << "Forward commands size: " << myForwardList.GetSize() << std::endl;
	std::cout << "Particle commands size: " << myParticleList.GetSize() << std::endl;
	std::cout << "Debug commands size: " << myDebugList.GetSize() << std::endl;
	std::cout << "Custom commands size: " << myCustomList.GetSize() << std::endl;
	std::cout << "Post process commands size: " << myPostProcessList.GetSize() << std::endl;
	std::cout << "Sprite commands size: " << mySpriteList.GetSize() << std::endl;
}

#include "GameEngine.pch.h"
#include "RenderQueue.h"

#include "../GraphicsEngine/GraphicsEngine.h"
#include "../GraphicsEngine/Commands/GCmdChangePipelineState.h"
#include "../GraphicsEngine/Commands/GCmdBeginEvent.h"
#include "../GraphicsEngine/Commands/GCmdEndEvent.h"
#include "../GraphicsEngine/Commands/GCmdSetVertexShader.h"
#include "../GraphicsEngine/Commands/GCmdSetPixelShader.h"

#include "../GraphicsEngine/PipelineStates.h"

#include <iostream>
#include "../GraphicsEngine/Commands/GCmdClearTextureResource.h"

RenderQueue::RenderQueue()
{
	Reset(true);
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

	if (myDebugList.HasCommands())
	{
		myDebugList.Execute();
	}
	myDebugList.Reset();

	if (myObjectIDRenderList.HasCommands())
	{
		myObjectIDRenderList.Execute();
	}
	myObjectIDRenderList.Reset();

	if (myParticleList.HasCommands())
	{
		myParticleList.Execute();
	}
	myParticleList.Reset();

	if (myWorldSpaceUIList.HasCommands())
	{
		myWorldSpaceUIList.Execute();
	}
	myWorldSpaceUIList.Reset();

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
		myObjectIDRenderList.Reset();
		myParticleList.Reset();
		myDebugList.Reset();
		myCustomList.Reset();
		myWorldSpaceUIList.Reset();
		myPostProcessList.Reset();
		mySpriteList.Reset();
	}
	myShadowList.Enqueue<GCmdEndEvent>();
	myShadowList.Enqueue<GCmdBeginEvent>("Dirlight Shadow Mapping");
	myShadowList.Enqueue<GCmdChangePipelineState>(static_cast<unsigned>(PipelineStates::DirlightShadowMapping));
	myShadowList.Enqueue<GCmdSetVertexShader>("Default_VS");
	myShadowList.Enqueue<GCmdSetPixelShader>("None");

	myDeferredList.Enqueue<GCmdEndEvent>();
	myDeferredList.Enqueue<GCmdBeginEvent>("Deferred");
	myDeferredList.Enqueue<GCmdChangePipelineState>(static_cast<unsigned>(PipelineStates::DeferredRendering));
	myDeferredList.Enqueue<GCmdSetVertexShader>("Default_VS");
	myDeferredList.Enqueue<GCmdSetPixelShader>("GBuffer_PS");

	myForwardList.Enqueue<GCmdEndEvent>();
	myForwardList.Enqueue<GCmdBeginEvent>("Forward");
	myForwardList.Enqueue<GCmdChangePipelineState>(static_cast<unsigned>(PipelineStates::ForwardRendering));
	myForwardList.Enqueue<GCmdSetVertexShader>("Default_VS");
	myForwardList.Enqueue<GCmdSetPixelShader>("Default_PS");

	myDebugList.Enqueue<GCmdEndEvent>();
	myDebugList.Enqueue<GCmdBeginEvent>("Debug");
	myDebugList.Enqueue<GCmdSetVertexShader>("Default_VS");
	myDebugList.Enqueue<GCmdSetPixelShader>("DebugLine_PS");

	myParticleList.Enqueue<GCmdClearTextureResource>(100);
	myParticleList.Enqueue<GCmdEndEvent>();
	myParticleList.Enqueue<GCmdBeginEvent>("Particles");

	myCustomList.Enqueue<GCmdEndEvent>();
	myCustomList.Enqueue<GCmdBeginEvent>("Custom");
	myCustomList.Enqueue<GCmdChangePipelineState>(static_cast<unsigned>(PipelineStates::CustomRendering));
	myCustomList.Enqueue<GCmdSetVertexShader>("Default_VS");
	myCustomList.Enqueue<GCmdSetPixelShader>("Default_PS");

	myWorldSpaceUIList.Enqueue<GCmdEndEvent>();
	myWorldSpaceUIList.Enqueue<GCmdBeginEvent>("WorldspaceUI");
	//myWorldSpaceUIList.Enqueue<GCmdChangePipelineState>(static_cast<unsigned>(PipelineStates::ParticleRendering));
	myWorldSpaceUIList.Enqueue<GCmdSetVertexShader>("Default_VS");
	myWorldSpaceUIList.Enqueue<GCmdSetPixelShader>("Default_PS");

	myObjectIDRenderList.Enqueue<GCmdEndEvent>();
	myObjectIDRenderList.Enqueue<GCmdBeginEvent>("ObjectID");
	myObjectIDRenderList.Enqueue<GCmdChangePipelineState>(static_cast<unsigned>(PipelineStates::ObjectIDRendering));
	myObjectIDRenderList.Enqueue<GCmdSetVertexShader>("Default_VS");
	myObjectIDRenderList.Enqueue<GCmdSetPixelShader>("ObjectID_PS");

	myPostProcessList.Enqueue<GCmdEndEvent>();
	myPostProcessList.Enqueue<GCmdBeginEvent>("PostProcessing");
	myPostProcessList.Enqueue<GCmdChangePipelineState>(static_cast<unsigned>(PipelineStates::CustomPostProcess));

	mySpriteList.Enqueue<GCmdEndEvent>();
	mySpriteList.Enqueue<GCmdBeginEvent>("UI");
	mySpriteList.Enqueue<GCmdChangePipelineState>(static_cast<unsigned>(PipelineStates::SpriteRendering));
	mySpriteList.Enqueue<GCmdSetVertexShader>("UI_VS");
	mySpriteList.Enqueue<GCmdSetPixelShader>("UI_PS");
}

void RenderQueue::PrintDebugInfo()
{
	std::cout << "Shadow commands size: " << myShadowList.GetSize() << std::endl;
	std::cout << "Deferred commands size: " << myDeferredList.GetSize() << std::endl;
	std::cout << "Forward commands size: " << myForwardList.GetSize() << std::endl;
	std::cout << "Particle commands size: " << myParticleList.GetSize() << std::endl;
	std::cout << "Debug commands size: " << myDebugList.GetSize() << std::endl;
	std::cout << "Custom commands size: " << myCustomList.GetSize() << std::endl;
	std::cout << "WorldspaceUI commands size: " << myWorldSpaceUIList.GetSize() << std::endl;
	std::cout << "Post process commands size: " << myPostProcessList.GetSize() << std::endl;
	std::cout << "Sprite commands size: " << mySpriteList.GetSize() << std::endl;
}

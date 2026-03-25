#pragma once

#include "../GraphicsEngine/Commands/GraphicsCommandList.h"
#include "RenderStage.h"

class RenderQueue
{
public:
	RenderQueue();
	~RenderQueue() = default;

	void SetStage(RenderStage aStage) { myCurrentStage = aStage; }

	template<typename CommandClass, typename ...Args>
	void Enqueue(Args&&... args)
	{
		switch (myCurrentStage)
		{
		case RenderStage::PreRendering:
			myPreList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderStage::ShadowMapping:
			myShadowList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderStage::Deferred:
			myDeferredList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderStage::Forward:
			myForwardList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderStage::Particles:
			myParticleList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderStage::Custom:
			myCustomList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderStage::WorldSpaceUI:
			myWorldSpaceUIList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderStage::PostProcess:
			myPostProcessList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderStage::Sprite:
			mySpriteList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderStage::ObjectIDRendering:
			myObjectIDRenderList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderStage::ObjectPartIDRendering:
			myObjectPartIDRenderList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderStage::UI:
			myUIList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		default:
			break;
		}
	}
	void RenderFrame();
	void Reset(bool aClearLists = false);

private:
	void PrintDebugInfo();

private:
	GraphicsCommandList myPreList;
	GraphicsCommandList myShadowList;
	GraphicsCommandList myDeferredList;
	GraphicsCommandList myForwardList;
	GraphicsCommandList myParticleList;
	GraphicsCommandList myCustomList;
	GraphicsCommandList myWorldSpaceUIList;
	GraphicsCommandList myPostProcessList;
	GraphicsCommandList mySpriteList;
	GraphicsCommandList myObjectIDRenderList;
	GraphicsCommandList myObjectPartIDRenderList;
	GraphicsCommandList myUIList;

	RenderStage myCurrentStage = RenderStage::ShadowMapping;
};


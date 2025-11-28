#pragma once

#include "../GraphicsEngine/Commands/GraphicsCommandList.h"

enum class RenderQueueStage
{
	ShadowMapping = 0,
	Deferred = 1,
	Forward = 2,
	Particles = 3,
	Debug = 4,
	Custom = 5,
	PostProcess = 6,
	Sprite = 7,
	Count = 8
};

class RenderQueue
{
public:
	RenderQueue();
	~RenderQueue() = default;

	void SetStage(RenderQueueStage aStage) { myCurrentStage = aStage; }

	template<typename CommandClass, typename ...Args>
	void Enqueue(Args&&... args)
	{
		switch (myCurrentStage)
		{
		case RenderQueueStage::ShadowMapping:
			myShadowList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderQueueStage::Deferred:
			myDeferredList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderQueueStage::Forward:
			myForwardList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderQueueStage::Particles:
			myParticleList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderQueueStage::Debug:
			myDebugList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderQueueStage::Custom:
			myCustomList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderQueueStage::PostProcess:
			myPostProcessList.Enqueue<CommandClass>(std::forward<Args>(args)...);
			break;
		case RenderQueueStage::Sprite:
			mySpriteList.Enqueue<CommandClass>(std::forward<Args>(args)...);
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
	GraphicsCommandList myShadowList;
	GraphicsCommandList myDeferredList;
	GraphicsCommandList myForwardList;
	GraphicsCommandList myParticleList;
	GraphicsCommandList myDebugList;
	GraphicsCommandList myCustomList;
	GraphicsCommandList myPostProcessList;
	GraphicsCommandList mySpriteList;

	RenderQueueStage myCurrentStage = RenderQueueStage::ShadowMapping;
};


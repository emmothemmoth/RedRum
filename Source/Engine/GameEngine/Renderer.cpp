#include "GameEngine.pch.h"
#include "Renderer.h"

Renderer::Renderer()
{
	//myUpdatePtr = &myFirstCmdList;
	//myIntermediatePtr = &mySecondCmdList;
	//myRenderPtr = &myThirdCmdList;

	myHasUpdated.store(false, std::memory_order_seq_cst);

	myUpdatePtr = &myFirstQueue;
	myUpdatePtr->Reset();
	myIntermediatePtr = &mySecondQueue;
	myIntermediatePtr->Reset();
	myRenderPtr = &myThirdQueue;
	myRenderPtr->Reset();
}

Renderer::~Renderer()
{
	myUpdatePtr = nullptr;
	myIntermediatePtr = nullptr;
	myRenderPtr = nullptr;
}


void Renderer::SwitchRenderIntermediate()
{
	//myBufferLock.lock();
	//GraphicsCommandList* tempIntermediate = myRenderPtr;
	//myRenderPtr = myIntermediatePtr;
	//myIntermediatePtr = tempIntermediate;
	//myHasUpdated = false;
	//myBufferLock.unlock();

	do
	{
		std::this_thread::yield();
	} while (myHasUpdated.load(std::memory_order_acquire) == false);

	RenderQueue* tempIntermediate = myRenderPtr;
	myRenderPtr = myIntermediatePtr;
	//myBufferLock.lock();
	myIntermediatePtr = tempIntermediate;
	//myBufferLock.unlock();
	tempIntermediate = nullptr;
	myHasUpdated.store(false, std::memory_order_release);
}



void Renderer::ChangeRenderPass(RenderQueueStage aStage)
{
	myCurrentStage = aStage;
}

void Renderer::SwitchUpdateIntermediate()
{
	//myBufferLock.lock();
	//GraphicsCommandList* tempIntermediate = myUpdatePtr;
	//myUpdatePtr = myIntermediatePtr;
	//myIntermediatePtr = tempIntermediate;
	//myUpdatePtr->Reset();
	//myHasUpdated = true;
	//myBufferLock.unlock();

	//do
	//{
	//	std::this_thread::yield();
	//} while (myHasUpdated);
	if (myHasUpdated.load(std::memory_order_acquire))
	{
		myUpdatePtr->Reset(true);
		return;
	}

	RenderQueue* tempIntermediate = myUpdatePtr;
	myUpdatePtr = myIntermediatePtr;
	//myBufferLock.lock();
	myIntermediatePtr = tempIntermediate;
	//myBufferLock.unlock();
	tempIntermediate = nullptr;
	myHasUpdated.store(true, std::memory_order_release);
}

void Renderer::RenderFrame()
{
	SwitchRenderIntermediate();

	myRenderPtr->RenderFrame();
	myRenderPtr->Reset();


	//if (myRenderPtr->HasCommands())
	//{
	//	myRenderPtr->Execute();
	//}
	//myRenderPtr->Reset();
}

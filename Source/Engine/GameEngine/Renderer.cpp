#include "GameEngine.pch.h"
#include "Renderer.h"
#include <iostream>

Renderer::Renderer()
{
    myUpdatePtr = &myFirstQueue;
    myIntermediatePtr = &mySecondQueue;
    myRenderPtr = &myThirdQueue;

    // ensure deterministic initial state
    myHasUpdated.store(false, std::memory_order_release);
}

Renderer::~Renderer()
{
    myUpdatePtr = nullptr;
    myIntermediatePtr = nullptr;
    myRenderPtr = nullptr;
}


void Renderer::SwitchRenderIntermediate()
{
    // Called by render thread to consume the latest intermediate buffer.
    std::unique_lock<std::mutex> lock(myBufferLock);

    // Wait until update publishes a new frame (myHasUpdated == true).
    myBufferCV.wait(lock, [this]()
        {
            return myHasUpdated.load(std::memory_order_acquire);
        });

    // Swap render <-> intermediate while holding lock to make it atomic.
    std::swap(myRenderPtr, myIntermediatePtr);

    // mark consumed
    myHasUpdated.store(false, std::memory_order_release);

    // Notify update thread that intermediate is free (if it was waiting)
    lock.unlock();
    myBufferCV.notify_one();
}

void Renderer::ChangeRenderPass(RenderStage aStage)
{
    myCurrentStage = aStage;
}

void Renderer::SwitchUpdateIntermediate()
{
    // Called by update thread after it finished populating myUpdatePtr.
    std::unique_lock<std::mutex> lock(myBufferLock);

    // Optional policy:
    // If you want to BLOCK the update thread until render has consumed previous frame,
    // use wait like this:
    // myBufferCV.wait(lock, [this]() { return !myHasUpdated.load(); });
    //
    // If you prefer to OVERWRITE intermediate (drop frames), skip waiting and publish immediately.
    //
    // Here we will *block* update if previous frame hasn't been consumed (safer/deterministic).
    myBufferCV.wait(lock, [this]()
        {
            return !myHasUpdated.load(std::memory_order_acquire);
        });

    // Swap update <-> intermediate (publish new frame)
    std::swap(myUpdatePtr, myIntermediatePtr);

    // Reset the new update buffer so update thread can append more commands next frame.
    myUpdatePtr->Reset();

    // Publish and notify render thread
    myHasUpdated.store(true, std::memory_order_release);

    // unlock before notify
    lock.unlock();
    myBufferCV.notify_one();
}

void Renderer::RenderFrame()
{
    // Wait for and swap in the next frame
    SwitchRenderIntermediate();

    // Now render what's in myRenderPtr
    //if (myRenderPtr->HasCommands())
    //{
    //    myRenderPtr->Execute();
    //}
    myRenderPtr->RenderFrame();

    // Reset the render buffer after executing it
    myRenderPtr->Reset(true);
}

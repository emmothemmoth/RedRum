#pragma once
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "RenderQueue.h"

constexpr unsigned MAX_RENDERED_OBJECTS = 2048;
#define RENDERER MainSingleton::Get().GetRenderer();

class Renderer
{
public:
    Renderer();
    ~Renderer();

    template<typename CommandClass, typename ...Args>
    void Enqueue(Args&&... args)
    {
        myUpdatePtr->SetStage(myCurrentStage);
        myUpdatePtr->Enqueue<CommandClass>(std::forward<Args>(args)...);
    }

    template<typename CommandClass, typename ...Args>
    void Enqueue(RenderStage aStage, Args&&... args)
    {
        myUpdatePtr->SetStage(aStage);
        myUpdatePtr->Enqueue<CommandClass>(std::forward<Args>(args)...);
        myUpdatePtr->SetStage(myCurrentStage);
    }

    void ChangeRenderPass(RenderStage aStage);
    void SwitchUpdateIntermediate(); // When Update is done
    void SwitchRenderIntermediate(); // When render is done
    void RenderFrame();
private:
    //GraphicsCommandList myFirstCmdList;
    //GraphicsCommandList mySecondCmdList;
    //GraphicsCommandList myThirdCmdList;
    //
    //GraphicsCommandList* myUpdatePtr;
    //GraphicsCommandList* myIntermediatePtr;
    //GraphicsCommandList* myRenderPtr;

    RenderQueue myFirstQueue;
    RenderQueue mySecondQueue;
    RenderQueue myThirdQueue;

    RenderQueue* myUpdatePtr;
    RenderQueue* myIntermediatePtr;
    RenderQueue* myRenderPtr;

    RenderStage myCurrentStage = RenderStage::ShadowMapping;

    // Synchronization
    std::atomic<bool> myHasUpdated = false;
    std::mutex myBufferLock;
    std::condition_variable myBufferCV;    // NEW
};
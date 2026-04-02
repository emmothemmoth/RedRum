#include "GraphicsEngine.pch.h"
#include "GCmdComputeMegaAcoustics.h"

#include "GraphicsEngine.h"

GCmdComputeMegaAcoustics::GCmdComputeMegaAcoustics(std::vector<GPURay> rays, std::vector<GPUObstacle> obs, std::vector<CU::Vector3f> probes, float probeRadius, CU::Vector3f lRight, std::shared_ptr<std::promise<std::vector<GPUMegaHit>>> promise)
    : myRays(std::move(rays)), myObstacles(std::move(obs)), myProbes(std::move(probes)),
    myProbeRadius(probeRadius), myListenerRight(lRight), myPromise(promise)
{}

void GCmdComputeMegaAcoustics::Execute()
{
    // 1. Declare the results at the top. If the GPU fails, this stays empty.
    std::vector<GPUMegaHit> finalResults;

    auto& ge = GraphicsEngine::Get();
    auto& rhi = ge.GetRHI();
    auto context = rhi->GetContext();

    // 2. Wrap all GPU logic inside a safety block
    if (ge.PrepareAcousticBuffers(myRays.size(), myObstacles.size(), myProbes.size()))
    {
        ID3D11Buffer* countBuf = ge.GetMegaHitCountBuffer().Get();
        ID3D11Buffer* stagingBuf = ge.GetMegaHitStagingBuffer().Get();
        ID3D11Buffer* hitBuf = ge.GetMegaHitBuffer().Get();
        ID3D11UnorderedAccessView* uav = ge.GetMegaHitUAV().Get();

        // Only proceed if all buffers were successfully created
        if (countBuf && stagingBuf && hitBuf && uav)
        {
            // --- UPLOAD ---
            rhi->UpdateDynamicBuffer(ge.GetAcousticRayBuffer().Get(), myRays);
            if (!myObstacles.empty()) rhi->UpdateDynamicBuffer(ge.GetAcousticObsBuffer().Get(), myObstacles);
            rhi->UpdateDynamicBuffer(ge.GetAcousticProbeBuffer().Get(), myProbes);

            AcousticSceneData sceneData;
            sceneData.ListenerRadius = myProbeRadius;
            sceneData.ListenerRight = myListenerRight;
            sceneData.ObstacleCount = static_cast<int>(myObstacles.size());
            sceneData.TotalProbeCount = static_cast<int>(myProbes.size());

            D3D11_MAPPED_SUBRESOURCE mappedCB;
            if (SUCCEEDED(context->Map(ge.GetAcousticSceneCB().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCB)))
            {
                memcpy(mappedCB.pData, &sceneData, sizeof(AcousticSceneData));
                context->Unmap(ge.GetAcousticSceneCB().Get(), 0);
            }

            // --- BIND & DISPATCH ---
            ge.SetComputeShader("ComputeAcoustics_CS");

            ID3D11Buffer* cbPtr = ge.GetAcousticSceneCB().Get();
            context->CSSetConstantBuffers(7, 1, &cbPtr);

            ID3D11ShaderResourceView* srvs[] = {
                ge.GetAcousticRaySRV().Get(),
                ge.GetAcousticObsSRV().Get(),
                ge.GetAcousticProbeSRV().Get()
            };
            context->CSSetShaderResources(6, 3, srvs);

            UINT initialCount = 0;
            context->CSSetUnorderedAccessViews(0, 1, &uav, &initialCount);

            rhi->Dispatch(static_cast<unsigned>((myRays.size() / 64) + 1), 1, 1);

            ID3D11UnorderedAccessView* nullUAV = nullptr;
            context->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);

            // --- READBACK ---
            context->CopyStructureCount(countBuf, 0, uav);

            D3D11_MAPPED_SUBRESOURCE mappedCount;
            unsigned int totalHits = 0;
            if (SUCCEEDED(context->Map(countBuf, 0, D3D11_MAP_READ, 0, &mappedCount)))
            {
                totalHits = *static_cast<unsigned int*>(mappedCount.pData);
                context->Unmap(countBuf, 0);
            }

            if (totalHits > 0)
            {
                unsigned int maxCap = static_cast<unsigned int>(ge.GetMegaHitCapacity());
                totalHits = (totalHits < maxCap) ? totalHits : maxCap;
                finalResults.resize(totalHits);

                context->CopyResource(stagingBuf, hitBuf);

                D3D11_MAPPED_SUBRESOURCE mappedData;
                if (SUCCEEDED(context->Map(stagingBuf, 0, D3D11_MAP_READ, 0, &mappedData)))
                {
                    memcpy(finalResults.data(), mappedData.pData, totalHits * sizeof(GPUMegaHit));
                    context->Unmap(stagingBuf, 0);
                }
            }
        }
    }

    // 3. THE PROMISE GUARANTEE
    // No matter what happens above (success, failure, null pointers), 
    // the worker thread will ALWAYS be woken up.
    if (myPromise)
    {
        myPromise->set_value(std::move(finalResults));
    }
}

void GCmdComputeMegaAcoustics::Destroy()
{
    myRays.clear();
    myObstacles.clear();
    myProbes.clear();
}

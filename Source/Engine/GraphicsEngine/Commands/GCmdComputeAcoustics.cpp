#include "GraphicsEngine.pch.h"
#include "GCmdComputeAcoustics.h"

#include "GraphicsEngine.h"

GCmdComputeAcoustics::GCmdComputeAcoustics(std::vector<GPURay> rays, std::vector<GPUObstacle> obs,
	CU::Vector3f lPos, CU::Vector3f lRight, float lRad,
	std::shared_ptr<std::promise<std::vector<GPURayResult>>> promise)
{
	myRays = rays;
	myObstacles = obs;
	myPromise = promise;
	myListenerPos = lPos;
	myListenerRight = lRight;
	myListenerRadius = lRad;
}

void GCmdComputeAcoustics::Execute()
{
	auto& ge = GraphicsEngine::Get();
	auto& rhi = ge.GetRHI();
	if (!ge.PrepareAcousticBuffers(myRays.size(), myObstacles.size()))
	{
		return;
	}
	rhi->UpdateDynamicBuffer(ge.GetAcousticRayBuffer().Get(), myRays);
	if (!myObstacles.empty())
	{
		rhi->UpdateDynamicBuffer(ge.GetAcousticObsBuffer().Get(), myObstacles);
	}
	AcousticSceneData sceneData;
	sceneData.ListenerPos = myListenerPos;
	sceneData.ListenerRight = myListenerRight;
	sceneData.ListenerRadius = myListenerRadius;
	sceneData.ObstacleCount = static_cast<int>(myObstacles.size());

	D3D11_MAPPED_SUBRESOURCE mappedCB;
	if (SUCCEEDED(rhi->GetContext()->Map(ge.GetAcousticSceneCB().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCB)))
	{
		memcpy(mappedCB.pData, &sceneData, sizeof(AcousticSceneData));
		rhi->GetContext()->Unmap(ge.GetAcousticSceneCB().Get(), 0);
	}

	ge.SetComputeShader("ComputeAcoustics_CS");
	ID3D11Buffer* cbPtr = ge.GetAcousticSceneCB().Get();
	rhi->GetContext()->CSSetConstantBuffers(7, 1, &cbPtr);
	rhi->SetCSShaderResource(6, ge.GetAcousticRaySRV());
	rhi->SetCSShaderResource(7, ge.GetAcousticObsSRV());
	rhi->SetCSUnorderedAccessView(0, ge.GetAcousticResultUAV());

	rhi->Dispatch(static_cast<unsigned>((myRays.size() / 64) + 1), 1, 1);

	rhi->SetCSUnorderedAccessView(0, nullptr);


	std::vector<GPURayResult> finalResults = rhi->ReadStructuredBuffer<GPURayResult>(
		ge.GetAcousticResultBuffer().Get(),
		ge.GetAcousticStagingBuffer().Get(),
		myRays.size());
	if (myPromise)
	{
		myPromise->set_value(std::move(finalResults));
	}
}

void GCmdComputeAcoustics::Destroy()
{
	myRays.clear();
	myObstacles.clear();
}

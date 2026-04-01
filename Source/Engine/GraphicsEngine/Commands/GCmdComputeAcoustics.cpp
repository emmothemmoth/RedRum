#include "GraphicsEngine.pch.h"
#include "GCmdComputeAcoustics.h"

#include "GraphicsEngine.h"

GCmdComputeAcoustics::GCmdComputeAcoustics(std::vector<GPURay> someRays, std::vector<GPUObstacle> someObstacles, std::shared_ptr<std::promise<std::vector<GPURayResult>>> aPromise)
{
	myRays = someRays;
	myObstacles = someObstacles;
	myPromise = aPromise;
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
	ge.SetComputeShader("ComputeAcoustics_cs");
	rhi->SetCSShaderResource(0, ge.GetAcousticRaySRV());
	rhi->SetCSShaderResource(1, ge.GetAcousticObsSRV());
	rhi->SetCSUnorderedAccessView(0, ge.GetAcousticResultUAV());

	rhi->Dispatch(static_cast<unsigned>((myRays.size() / 64) + 1), 1, 1);

	rhi->SetCSUnorderedAccessView(0, nullptr);
	ComPtr<ID3D11Buffer> stagingBuffer;
	rhi->CreateStagingBuffer(sizeof(GPURayResult) * myRays.size(), stagingBuffer);

	std::vector<GPURayResult> finalResults = rhi->ReadStructuredBuffer<GPURayResult>(
		ge.GetAcousticResultBuffer().Get(),
		stagingBuffer.Get(),
		myRays.size()
	);
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

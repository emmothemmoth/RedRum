#include "GraphicsEngine.pch.h"
#include "GCmdScreenPicking.h"

#include "GraphicsEngine.h"

GCmdScreenPicking::GCmdScreenPicking(const unsigned anX, const unsigned aY, std::shared_ptr<std::promise<uint32_t>> aPromise)
{
	myX = anX;
	myY = aY;
	myPromise = aPromise;
}

void GCmdScreenPicking::Execute()
{
	auto& ge = GraphicsEngine::Get();
	ID3D11Resource* gpuTex = ge.GetObjectIDTexture()->GetResource().Get();
	ID3D11Resource* stagingTex = ge.GetScreenPickingTexture()->GetResource().Get();
	uint32_t hitID = ge.GetRHI()->ReadTexture2DPixel<uint32_t>(gpuTex, stagingTex, myX, myY);

	if (myPromise)
	{
		myPromise->set_value(hitID);
	}
}

void GCmdScreenPicking::Destroy()
{
}

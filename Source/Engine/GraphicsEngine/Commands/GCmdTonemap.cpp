#include "GraphicsEngine.pch.h"
#include "GCmdTonemap.h"

#include "GraphicsEngine.h"
#include "PipelineStates.h"

GCmdTonemap::GCmdTonemap()
{
}

void GCmdTonemap::Execute()
{
	auto& GE = GraphicsEngine::Get();
	GE.BeginEvent("Tonemapping");
	GE.SetPixelShader("Tonemap_PS");
	GE.SetVertexShader("Quad_VS");
	GE.ChangePipelineState(static_cast<int>(PipelineStates::Tonemap));
	GE.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 126, *GE.GetHDRBuffer());
	GE.DrawQuad();
	GE.EndEvent();
	GE.ClearTextureResourceSlot(static_cast<unsigned>(PIPELINE_STAGE_PIXEL_SHADER), 126);
}

void GCmdTonemap::Destroy()
{
}

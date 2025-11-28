#include "GraphicsEngine.pch.h"
#include "GCmdSSAO.h"

#include "GraphicsEngine.h"
#include "PipelineStates.h"
GCmdSSAO::GCmdSSAO()
{
}

void GCmdSSAO::Execute()
{
	GraphicsEngine& ge = GraphicsEngine::Get();
	ge.BeginEvent("SSAO");
	//Sample from "backbuffer" 
	ge.ChangePipelineState(static_cast<int>(PipelineStates::SSAO));
	ge.ChangeRenderTarget(GraphicsEngine::Get().GetSSAOBuffer());
	ge.SetVertexShader("Quad_VS");
	ge.SetPixelShader("SSAO_PS");
	ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 4, *GraphicsEngine::Get().GetBlueNoiseTexture());
	ge.DrawQuad();
	ge.ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 4);

	ge.SetMarker("Downsampling");
	ge.SetPixelShader("Resample_PS");
	ge.ChangePipelineState(static_cast<int>(PipelineStates::Resampling));
	ge.ChangeRenderTarget(GraphicsEngine::Get().GetHalfSizeBuffer());
	ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 20, *GraphicsEngine::Get().GetSSAOBuffer());
	ge.DrawQuad(GraphicsEngine::Get().GetHalfSizeBuffer()->GetSize());
	ge.ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 20);

	ge.SetMarker("Blur");
	ge.SetPixelShader("SSAO_Radial_Blur_PS");
	ge.ChangeRenderTarget(GraphicsEngine::Get().GetHalfSizeBufferB());
	ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 119, *GraphicsEngine::Get().GetHalfSizeBuffer());
	ge.DrawQuad(GraphicsEngine::Get().GetHalfSizeBufferB()->GetSize());
	ge.ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 119);

	ge.SetMarker("Upsampling");
	ge.SetPixelShader("Resample_PS");
	ge.ChangePipelineState(static_cast<int>(PipelineStates::Resampling));
	ge.ChangeRenderTarget(GraphicsEngine::Get().GetSSAOBuffer());
	ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 20, *GraphicsEngine::Get().GetHalfSizeBufferB());
	ge.DrawQuad(GraphicsEngine::Get().GetSSAOBuffer()->GetSize());
	ge.ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 20);
	ge.ClearRenderTarget();

	//ge.ClearRenderTarget();
	ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,119, *GraphicsEngine::Get().GetSSAOBuffer());


	//Downsample to halfsize

	//Blur

	//Upsample again

	//TODO: need to set the SSAO buffer before light gets rendered to slot 119
}

void GCmdSSAO::Destroy()
{
}

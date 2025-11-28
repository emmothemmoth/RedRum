#include "GraphicsEngine.pch.h"
#include "GCmdBloom.h"

#include "GraphicsEngine.h"
#include "PipelineStates.h"

GCmdBloom::GCmdBloom()
{
}

void GCmdBloom::Execute()
{
	GraphicsEngine& ge = GraphicsEngine::Get();
	ge.BeginEvent("Bloom");
	
	ge.ChangePipelineState(static_cast<int>(PipelineStates::Luminance));
	ge.SetVertexShader("Quad_VS");
	ge.SetPixelShader("Luminance_PS");
	ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 126, *GraphicsEngine::Get().GetLDRBuffer());
	ge.DrawQuad();
	ge.ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 126);


	ge.SetMarker("Downsampling");
	ge.SetPixelShader("Resample_PS");
	ge.ChangePipelineState(static_cast<int>(PipelineStates::Resampling)); 
	ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 20, *GraphicsEngine::Get().GetLuminanceBuffer());
	ge.ChangeRenderTarget(GraphicsEngine::Get().GetHalfSizeBuffer());
	ge.DrawQuad(GraphicsEngine::Get().GetHalfSizeBuffer()->GetSize());
	ge.ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 20);
	
	ge.ChangeRenderTarget(GraphicsEngine::Get().GetQuarterSizeBuffer());
	ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 20, *GraphicsEngine::Get().GetHalfSizeBuffer());
	ge.DrawQuad(GraphicsEngine::Get().GetQuarterSizeBuffer()->GetSize());
	ge.ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 20);
	
	ge.ChangeRenderTarget(GraphicsEngine::Get().GetEightSizeBufferA());
	ge.SetTextureResource((PIPELINE_STAGE_PIXEL_SHADER), 20, *GraphicsEngine::Get().GetQuarterSizeBuffer());
	ge.DrawQuad(GraphicsEngine::Get().GetEightSizeBufferA()->GetSize());
	ge.ClearTextureResourceSlot((PIPELINE_STAGE_PIXEL_SHADER), 20);

	for (int index = 0; index < 10; index++)
	{
		ge.SetMarker("Blur");
		ge.SetPixelShader("GaussianH_PS");
		ge.ChangePipelineState(static_cast<int>(PipelineStates::GaussianH));
		ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 20, *GraphicsEngine::Get().GetEightSizeBufferA());
		ge.DrawQuad(GraphicsEngine::Get().GetEightSizeBufferB()->GetSize());
		ge.ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 20);

		ge.SetPixelShader("GaussianV_PS");
		ge.ChangePipelineState(static_cast<int>(PipelineStates::GaussianV));
		ge.SetTextureResource((PIPELINE_STAGE_PIXEL_SHADER), 20, *GraphicsEngine::Get().GetEightSizeBufferB());
		ge.DrawQuad(GraphicsEngine::Get().GetEightSizeBufferA()->GetSize());
		ge.ClearTextureResourceSlot((PIPELINE_STAGE_PIXEL_SHADER), 20);
	}


	ge.SetMarker("Upsampling");
	ge.SetPixelShader("Resample_PS");
	ge.ChangePipelineState(static_cast<int>(PipelineStates::Resampling));
	ge.ChangeRenderTarget(GraphicsEngine::Get().GetQuarterSizeBuffer()); 
	ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 20, *GraphicsEngine::Get().GetEightSizeBufferA());
	ge.DrawQuad(GraphicsEngine::Get().GetQuarterSizeBuffer()->GetSize());
	ge.ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 20);
	
	ge.ChangeRenderTarget(GraphicsEngine::Get().GetHalfSizeBuffer()); 
	ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 20, *GraphicsEngine::Get().GetQuarterSizeBuffer());
	ge.DrawQuad(GraphicsEngine::Get().GetHalfSizeBuffer()->GetSize());
	ge.ClearTextureResourceSlot((PIPELINE_STAGE_PIXEL_SHADER), 20);
	
	ge.ChangeRenderTarget(GraphicsEngine::Get().GetLuminanceBuffer());
	ge.SetTextureResource((PIPELINE_STAGE_PIXEL_SHADER), 20, *GraphicsEngine::Get().GetHalfSizeBuffer());
	ge.DrawQuad();
	ge.ClearTextureResourceSlot((PIPELINE_STAGE_PIXEL_SHADER), 20);
	
	ge.SetMarker("Blending");
	ge.SetPixelShader("Bloom_PS");
	ge.ChangePipelineState(static_cast<int>(PipelineStates::Bloom));
	ge.SetTextureResource((PIPELINE_STAGE_PIXEL_SHADER), 20, *GraphicsEngine::Get().GetLuminanceBuffer());
	ge.SetTextureResource((PIPELINE_STAGE_PIXEL_SHADER), 125, *GraphicsEngine::Get().GetLDRBuffer());
	ge.DrawQuad();
	ge.ClearTextureResourceSlot((PIPELINE_STAGE_PIXEL_SHADER), 20);
	ge.ClearTextureResourceSlot((PIPELINE_STAGE_PIXEL_SHADER), 125);
	ge.EndEvent();
}

void GCmdBloom::Destroy()
{
}

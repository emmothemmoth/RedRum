#include "GraphicsEngine.pch.h"
#include "GCmdRenderParticles.h"

#include "../Objects/ParticleSystem.h"
#include "../Objects/ParticleEmitter.h"
#include "../Objects/ParticleEmitterSettings.h"
#include "../Buffers/ObjectBuffer.h"

#include "../PipelineStates.h"

GCmdRenderParticles::GCmdRenderParticles(CU::Matrix4x4f aTransform, std::shared_ptr<ParticleSystem> aParticleSystem)
{
	myParticleSystem = aParticleSystem;
	myTransform = aTransform;
}

void GCmdRenderParticles::Execute()
{
	ObjectBufferData objectBuffer;
	objectBuffer.Transform = myTransform;
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objectBuffer);
	GraphicsEngine::Get().ChangePipelineState(static_cast<unsigned>(PipelineStates::ParticleRendering));
	GraphicsEngine::Get().SetVertexShader("Particle_VS");
	GraphicsEngine::Get().SetGeometryShader("Particle_GS");
	GraphicsEngine::Get().SetPixelShader("Particle_PS");

	for (auto& emitter : myParticleSystem->GetEmitters())
	{
		std::shared_ptr<ParticleEmitterSettings> settings = emitter->GetSettings();
		for (const auto& [slot, texture] : settings->Textures)
		{
			GraphicsEngine::Get().SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, slot, *texture);
		}
		GraphicsEngine::Get().PrepareParticleEmitter(*emitter);
		GraphicsEngine::Get().SetIndexBuffer(nullptr);
		GraphicsEngine::Get().SetVertexBuffer(emitter->GetVertexBuffer()->Buffer);
		GraphicsEngine::Get().RenderPoints(static_cast<unsigned>(emitter->GetParticleVertices().size()));
		for (auto& [slot, texture] : settings->Textures)
		{
			GraphicsEngine::Get().ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, slot);
		}
	}
	GraphicsEngine::Get().SetGeometryShader("");
}

void GCmdRenderParticles::Destroy()
{
	myParticleSystem = nullptr;
}

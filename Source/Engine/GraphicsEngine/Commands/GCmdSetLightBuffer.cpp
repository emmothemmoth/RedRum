#include "GraphicsEngine.pch.h"
#include "GCmdSetLightBuffer.h"

#include "Buffers\LightBuffer.h"
#include "GraphicsEngine.h"


GCmdSetLightBuffer::GCmdSetLightBuffer(std::shared_ptr<LightBuffer> someLightSources)
{
	myLightBuffer = someLightSources;
}

void GCmdSetLightBuffer::Execute()
{
	//LightBufferData& lightBuffer = GetLightBuffer();
	//lightBuffer.Data.directionalLight = myLightSources.DirectionalLightSource;;
	//lightBuffer.Data.pointLights = myLightSources.PointLightSources;
	//lightBuffer.Data.spotLights = myLightSources.SpotLightSources;
	//
	//GraphicsEngine::Get().UpdateConstantBuffer(lightBuffer);
	//GraphicsEngine::Get().SetConstantBuffer(PIPELINE_STAGE_PIXEL_SHADER, 2, lightBuffer);
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::LightBuffer, *myLightBuffer);
}

void GCmdSetLightBuffer::Destroy()
{
	myLightBuffer = nullptr;
}

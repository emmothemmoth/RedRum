#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"

#include "GCmdSetTextureResource.h"
#include "Objects/TextureAsset.h"



GCmdSetTextureResource::GCmdSetTextureResource(std::shared_ptr<TextureAsset> aTexture, unsigned int aSlot)
{
	myTexture = aTexture;
	mySlot = aSlot;
}

void GCmdSetTextureResource::Execute()
{
	GraphicsEngine::Get().SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER | PIPELINE_STAGE_VERTEX_SHADER, mySlot, *myTexture);
}

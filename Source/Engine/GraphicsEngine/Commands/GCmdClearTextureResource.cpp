#include "GraphicsEngine.pch.h"
#include "GCmdClearTextureResource.h"

#include "GraphicsEngine.h"

GCmdClearTextureResource::GCmdClearTextureResource(unsigned int aSlot)
{
	mySlot = aSlot;
}

void GCmdClearTextureResource::Execute()
{
	GraphicsEngine::Get().ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, mySlot);
}

void GCmdClearTextureResource::Destroy()
{
}

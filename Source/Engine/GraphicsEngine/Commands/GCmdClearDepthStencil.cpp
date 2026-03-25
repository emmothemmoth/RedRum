#include "GraphicsEngine.pch.h"
#include "GCmdClearDepthStencil.h"

#include "GraphicsEngine.h"

GCmdClearDepthStencil::GCmdClearDepthStencil()
{
}

void GCmdClearDepthStencil::Execute()
{
	GraphicsEngine::Get().ClearDepthBuffer();
}

void GCmdClearDepthStencil::Destroy()
{
}

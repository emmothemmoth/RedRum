#include "GraphicsEngine.pch.h"
#include "GCmdClearBackBuffer.h"

#include "GraphicsEngine.h"

GCmdClearBackBuffer::GCmdClearBackBuffer()
{
}

void GCmdClearBackBuffer::Execute()
{
	GraphicsEngine::Get().ClearBackBuffer();
}

void GCmdClearBackBuffer::Destroy()
{
}

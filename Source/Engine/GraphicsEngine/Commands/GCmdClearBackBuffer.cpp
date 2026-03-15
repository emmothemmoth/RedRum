#include "GraphicsEngine.pch.h"
#include "GCmdClearBackBuffer.h"

#include "GraphicsEngine.h"

GCmdClearBackBuffer::GCmdClearBackBuffer(bool aClearActualBackbuffer)
{
	myShouldClearActualBackbuffer = aClearActualBackbuffer;
}

void GCmdClearBackBuffer::Execute()
{
	if (myShouldClearActualBackbuffer)
	{
		GraphicsEngine::Get().ClearBackBuffer();
	}
	else
	{
		GraphicsEngine::Get().ClearViewportBackBuffer();
	}
}

void GCmdClearBackBuffer::Destroy()
{
}

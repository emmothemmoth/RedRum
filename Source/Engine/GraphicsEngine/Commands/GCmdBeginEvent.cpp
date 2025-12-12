#include "GraphicsEngine.pch.h"
#include "GCmdBeginEvent.h"

#include "GraphicsEngine.h"

void GCmdBeginEvent::Execute()
{
	GraphicsEngine::Get().BeginEvent(myName);
}

void GCmdBeginEvent::Destroy()
{
}

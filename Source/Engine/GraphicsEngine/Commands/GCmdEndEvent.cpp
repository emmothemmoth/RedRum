#include "GraphicsEngine.pch.h"
#include "GCmdEndEvent.h"

#include "GraphicsEngine.h"

void GCmdEndEvent::Execute()
{
	GraphicsEngine::Get().EndEvent();
}

void GCmdEndEvent::Destroy()
{
}

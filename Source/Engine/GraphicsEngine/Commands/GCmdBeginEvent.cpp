#include "GraphicsEngine.pch.h"
#include "GCmdBeginEvent.h"

#include "GraphicsEngine.h"

GCmdBeginEvent::GCmdBeginEvent(std::string_view aName)
{
	myName = aName;
}

void GCmdBeginEvent::Execute()
{
	GraphicsEngine::Get().BeginEvent(myName);
}

void GCmdBeginEvent::Destroy()
{
}

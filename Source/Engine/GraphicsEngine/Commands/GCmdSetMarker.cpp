#include "GraphicsEngine.pch.h"
#include "GCmdSetMarker.h"

#include "GraphicsEngine.h"

void GCmdSetMarker::Execute()
{
	GraphicsEngine::Get().SetMarker(myMarker);
}

void GCmdSetMarker::Destroy()
{
}

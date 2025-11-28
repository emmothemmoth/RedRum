#include "GraphicsEngine.pch.h"
#include "GCmdPresent.h"

#include "GraphicsEngine.h"

GCmdPresent::GCmdPresent()
{
}

void GCmdPresent::Execute()
{
	GraphicsEngine::Get().EndFrame();
}

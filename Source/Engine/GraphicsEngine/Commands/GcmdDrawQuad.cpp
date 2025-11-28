#include "GraphicsEngine.pch.h"
#include "GcmdDrawQuad.h"

#include "GraphicsEngine.h"
GCmdDrawQuad::GCmdDrawQuad(const CU::Vector2f& aSize)
{
	mySize = aSize;
}

void GCmdDrawQuad::Execute()
{
	GraphicsEngine::Get().DrawQuad(mySize);
}

void GCmdDrawQuad::Destroy()
{
}

#include "GraphicsEngine.pch.h"
#include "GCmdScreenPicking.h"

#include "GraphicsEngine.h"

GCmdScreenPicking::GCmdScreenPicking(const unsigned anX, const unsigned aY)
{
	myX = anX;
	myY = aY;
}

void GCmdScreenPicking::Execute()
{
	GraphicsEngine::Get().GetIDFromPoint(myX, myY);
}

void GCmdScreenPicking::Destroy()
{
}

#include "GraphicsEngine.pch.h"
#include "GCmdCustom.h"

GCmdCustom::GCmdCustom(RenderFunction aFunction)
	: myFunction(aFunction)
{
}

void GCmdCustom::Execute()
{
	if (myFunction)
	{
		myFunction();
	}
}

void GCmdCustom::Destroy()
{
}

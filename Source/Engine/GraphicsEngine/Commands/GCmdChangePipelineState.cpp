#include "GraphicsEngine.pch.h"
#include "GCmdChangePipelineState.h"

#include "InterOp\PipelineStateObject.h"
#include "GraphicsEngine.h"

GCmdChangePipelineState::GCmdChangePipelineState(unsigned aPipelineState)
{
	myPipelineState = aPipelineState;
}

void GCmdChangePipelineState::Execute()
{
	GraphicsEngine::Get().ChangePipelineState(myPipelineState);
}

void GCmdChangePipelineState::Destroy()
{
}

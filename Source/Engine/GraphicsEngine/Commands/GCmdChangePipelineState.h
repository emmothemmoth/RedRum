#pragma once

#include "GraphicsCommandList.h"
#include <memory>

struct PipelineStateObject;

class GCmdChangePipelineState : public GraphicsCommandBase
{
public:
	GCmdChangePipelineState(unsigned aPipelineState);
	~GCmdChangePipelineState() override = default;

	void Execute() override;
	void Destroy() override;

private:
	unsigned myPipelineState;
};
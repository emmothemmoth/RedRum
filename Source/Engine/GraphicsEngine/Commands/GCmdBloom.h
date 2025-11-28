#pragma once
#include "GraphicsCommandList.h"

class GCmdBloom : public GraphicsCommandBase
{
public:
	GCmdBloom();
	~GCmdBloom() override = default;
	void Execute() override;
	void Destroy() override;

private:
};


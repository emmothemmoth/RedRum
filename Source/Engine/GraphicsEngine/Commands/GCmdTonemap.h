#pragma once
#include "GraphicsCommandList.h"

class GCmdTonemap : public GraphicsCommandBase
{
public:
	GCmdTonemap();
	~GCmdTonemap() override = default;
	void Execute() override;
	void Destroy() override;

private:
};

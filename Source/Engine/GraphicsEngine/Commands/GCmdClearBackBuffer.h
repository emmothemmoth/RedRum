#pragma once

#include "GraphicsCommandList.h"

class GCmdClearBackBuffer : public GraphicsCommandBase
{
public:
	GCmdClearBackBuffer(bool aClearActualBackbuffer = false);
	~GCmdClearBackBuffer() override = default;

	void Execute() override;
	void Destroy() override;
private:
	bool myShouldClearActualBackbuffer = false;
};

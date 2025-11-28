#pragma once

#include "GraphicsCommandList.h"

class GCmdClearBackBuffer : public GraphicsCommandBase
{
public:
	GCmdClearBackBuffer();
	~GCmdClearBackBuffer() override = default;

	void Execute() override;
	void Destroy() override;
};

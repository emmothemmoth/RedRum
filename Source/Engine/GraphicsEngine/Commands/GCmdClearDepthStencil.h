#pragma once
#include "GraphicsCommandList.h"


class GCmdClearDepthStencil : public GraphicsCommandBase
{
public:
	GCmdClearDepthStencil();
	~GCmdClearDepthStencil() override = default;

	void Execute() override;
	void Destroy() override;
private:
};
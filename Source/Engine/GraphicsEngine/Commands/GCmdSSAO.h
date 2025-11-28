#pragma once
#include "GraphicsCommandList.h"

class GCmdSSAO : public GraphicsCommandBase
{
public:
	GCmdSSAO();
	~GCmdSSAO() override = default;
	void Execute() override;
	void Destroy() override;

private:
};

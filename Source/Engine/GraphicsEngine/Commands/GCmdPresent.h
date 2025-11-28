#pragma once
#include "GraphicsCommandList.h"

class GCmdPresent : public GraphicsCommandBase
{
public:
	GCmdPresent();
	~GCmdPresent() override = default;

	void Execute() override;
};
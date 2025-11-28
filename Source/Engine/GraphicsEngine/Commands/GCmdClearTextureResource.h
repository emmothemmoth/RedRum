#pragma once

#include "GraphicsCommandList.h"

class GCmdClearTextureResource : public GraphicsCommandBase
{
public:
	GCmdClearTextureResource(unsigned int aSlot);

	void Execute() override;
	void Destroy() override;

private:
	unsigned int mySlot;
};


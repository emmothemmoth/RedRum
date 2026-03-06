#pragma once
#include "GraphicsCommandList.h"
class GCmdScreenPicking : public GraphicsCommandBase
{
public:
	GCmdScreenPicking(const unsigned anX, const unsigned aY);
	~GCmdScreenPicking() override = default;
	void Execute() override;
	void Destroy() override;

private:
	unsigned myX;
	unsigned myY;
};

#pragma once

#include "GraphicsCommandList.h"
class GCmdCustom : public GraphicsCommandBase
{
public:
	using RenderFunction = std::function<void()>;
	GCmdCustom(RenderFunction aFunction);
	~GCmdCustom() override = default;
	void Execute() override;
	void Destroy() override;

private:
	RenderFunction myFunction;
};


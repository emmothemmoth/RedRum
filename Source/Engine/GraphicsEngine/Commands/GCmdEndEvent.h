#pragma once
#include "GraphicsCommandList.h"
class GCmdEndEvent : public GraphicsCommandBase
{
public:
	GCmdEndEvent() = default;
	~GCmdEndEvent() override = default;
	void Execute() override;
	void Destroy() override;

private:
	std::string_view myName;
};


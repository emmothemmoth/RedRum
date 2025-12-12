#pragma once
#include "GraphicsCommandList.h"

class GCmdBeginEvent : public GraphicsCommandBase
{
public:
	explicit GCmdBeginEvent(std::string_view aName) { myName = aName; }
	~GCmdBeginEvent() override = default;
	void Execute() override;
	void Destroy() override;

private:
	std::string_view myName;
};


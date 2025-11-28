#pragma once
#include "GraphicsCommandList.h"


enum class RenderQueueStage;

class GCmdBeginEvent : public GraphicsCommandBase
{
public:
	explicit GCmdBeginEvent(std::string_view aName, RenderQueueStage aRenderQueueStage);
	~GCmdBeginEvent() override = default;
	void Execute() override;
	void Destroy() override;

private:
	std::string_view myName;
};


#pragma once
#include "../Utilities/CommonUtilities\Matrix.hpp"
#include "GraphicsCommandList.h"

struct FrameBufferData;

class GCmdSetFrameBuffer : public GraphicsCommandBase
{
public:
	GCmdSetFrameBuffer(const std::shared_ptr<FrameBufferData> aFrameBuffer);
	~GCmdSetFrameBuffer() override = default;

	void Execute() override;
	void Destroy() override;

private:
	std::shared_ptr<FrameBufferData> myFrameBuffer;
};


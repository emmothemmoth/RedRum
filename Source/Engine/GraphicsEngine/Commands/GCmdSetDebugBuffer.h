#pragma once
#include "GraphicsCommandList.h"

#include <memory>
struct DebugBuffer;

class GCmdSetDebugBuffer : public GraphicsCommandBase
{
public:
	GCmdSetDebugBuffer(std::shared_ptr<DebugBuffer> aBuffer);
	~GCmdSetDebugBuffer() = default;
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<DebugBuffer> myBuffer;
};

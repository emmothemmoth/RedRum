#pragma once

#include "GraphicsCommandList.h"

class GCmdSetMarker : public GraphicsCommandBase
{
public:
	explicit GCmdSetMarker(std::string_view aMarker) { myMarker = aMarker; }
	~GCmdSetMarker() override = default;
	void Execute() override;
	void Destroy() override;

private:
	std::string_view myMarker;
};

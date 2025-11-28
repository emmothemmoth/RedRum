#pragma once
#include <memory>
#include<vector>

#include "GraphicsCommandList.h"

struct LightBuffer;


class GCmdSetLightBuffer : public GraphicsCommandBase
{
public:
	GCmdSetLightBuffer(std::shared_ptr<LightBuffer> someLightSources);

	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<LightBuffer> myLightBuffer;
};

#pragma once
#include "GraphicsCommandList.h"
#include <future>
#include <memory>
class GCmdScreenPicking : public GraphicsCommandBase
{
public:
	GCmdScreenPicking(const unsigned anX, const unsigned aY, std::shared_ptr<std::promise<uint32_t>> aPromise);
	~GCmdScreenPicking() override = default;
	void Execute() override;
	void Destroy() override;

private:
	std::shared_ptr<std::promise<uint32_t>> myPromise;
	unsigned myX;
	unsigned myY;
};

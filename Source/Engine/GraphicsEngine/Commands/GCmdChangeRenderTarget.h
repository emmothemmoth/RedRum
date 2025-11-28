#pragma once

#include "GraphicsCommandList.h"

#include <memory>

class TextureAsset;

class GCmdChangeRenderTarget : public GraphicsCommandBase
{
public:
	GCmdChangeRenderTarget(std::shared_ptr<TextureAsset> aRenderTarget);
	~GCmdChangeRenderTarget() override = default;

	void Execute() override;
	void Destroy() override;

private:
	std::shared_ptr<TextureAsset> myRenderTarget;
};

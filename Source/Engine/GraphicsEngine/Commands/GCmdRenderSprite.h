#pragma once
#include "GraphicsCommandList.h"

class SpriteAsset;

class GCmdRenderSprite : public GraphicsCommandBase
{
public:
	GCmdRenderSprite(SpriteAsset* aElement);
	~GCmdRenderSprite() override;

	void Execute() override;
	void Destroy() override;

private:
	SpriteAsset* myElement;
};

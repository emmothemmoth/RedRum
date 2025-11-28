#pragma once
#include "GraphicsCommandList.h"

class TextureAsset;

class GCmdSetTextureResource : public GraphicsCommandBase
{
public:
	GCmdSetTextureResource(std::shared_ptr<TextureAsset> aTexture, unsigned int aSlot);

	void Execute() override;

private:
	std::shared_ptr<TextureAsset> myTexture;
	unsigned int mySlot;
};



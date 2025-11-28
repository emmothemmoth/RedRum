#pragma once
#include "Objects\TextureAsset.h"

struct GBufferData
{
	std::shared_ptr<TextureAsset> Albedo;
	std::shared_ptr<TextureAsset> WorldNormal;
	std::shared_ptr<TextureAsset> Material;
	std::shared_ptr<TextureAsset> WorldPosition;
	std::shared_ptr<TextureAsset> FXTexture;
};

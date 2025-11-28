#pragma once
#include "Component.h"


class TextureAsset;

class TextureComponent : public Component
{
public:
	TextureComponent() = delete;
	//TextureComponent(GameObject& aParent, std::weak_ptr<TextureAsset> aTexture);
	TextureComponent(GameObject& aParent, std::shared_ptr<TextureAsset> aTexture);
	~TextureComponent();

	//std::weak_ptr<TextureAsset> GetTexture();
	std::shared_ptr<TextureAsset> GetTexture();

private:
	//std::weak_ptr<TextureAsset> myTexture;
	std::shared_ptr<TextureAsset> myTexture;
};


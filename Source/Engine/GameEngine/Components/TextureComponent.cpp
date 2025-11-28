#include "GameEngine.pch.h"
#include "TextureComponent.h"

#include "..\GraphicsEngine\Objects\TextureAsset.h"

TextureComponent::TextureComponent(GameObject& aParent, std::shared_ptr<TextureAsset> aTexture)
	: Component(aParent)
	,myTexture(aTexture)
{
	//myID = ComponentID::Texture;
}

TextureComponent::~TextureComponent()
{
}

std::shared_ptr<TextureAsset> TextureComponent::GetTexture()
{
	return myTexture;
}

#pragma once

#include "Component.h"

class TextureAsset;
class BillboardComponent : public Component
{
public:
	BillboardComponent(GameObject& aParent);
	~BillboardComponent();

	void SetTexture(std::shared_ptr<TextureAsset> aTexture);

	void SetOffset(const CU::Vector4f& anOffset);

	void Render() override;

private:
	std::shared_ptr<TextureAsset> myTexture;
	CU::Vector4f myOffset;

};


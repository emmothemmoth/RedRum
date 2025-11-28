

#include "GraphicsEngine.pch.h"
#include "TextureAsset.h"
#include "../../AssetManager/TextureAssetHandler.h"

#include "InterOp\RHI.h"

 TextureAsset::TextureAsset(const std::filesystem::path& aPath)
{
	 myPath = aPath;
	 myName = aPath.stem().string();
}


 bool TextureAsset::Load()
{
	if (TextureAssetHandler::Get().LoadTextureFromFile(myPath, *this))
	{
		return true;
	}
	return false;
}

 CU::Vector2<unsigned> TextureAsset::GetTextureSize() const
 {
	 ID3D11Resource* resource = nullptr;
	 mySRV->GetResource(&resource);

	 if (!resource)
	 {
		 return { 0,0 };
	 }

	 ID3D11Texture2D* texture = reinterpret_cast<ID3D11Texture2D*>(resource);
	 D3D11_TEXTURE2D_DESC desc;
	 texture->GetDesc(&desc);

	 CU::Vector2<unsigned> size;
	 size.x = desc.Width;
	 size.y = desc.Height;

	 resource->Release();

	 return size;
 }

 void TextureAsset::SetName(const std::string& aName)
 {
	 myName = aName;
 }

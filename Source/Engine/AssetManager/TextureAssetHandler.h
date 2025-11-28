#pragma once
#include <string>

class TextureAsset;
class TextureAssetHandler
{
public:
	static TextureAssetHandler& Get() { static TextureAssetHandler myInstance; return myInstance; }

	bool LoadTextureFromFile(const std::wstring& aPath, TextureAsset& inOutAsset) const;

private:

};


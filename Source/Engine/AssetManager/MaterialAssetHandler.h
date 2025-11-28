#pragma once

class TextureAsset;
class MaterialAsset;

class MaterialAssetHandler
{
public:
	static MaterialAssetHandler& Get() { static MaterialAssetHandler myInstance; return myInstance; }

	bool LoadMaterialFromJSon(const std::filesystem::path& aPath, MaterialAsset& inOutAsset);

private:
};


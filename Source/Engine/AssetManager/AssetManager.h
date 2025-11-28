#pragma once
#include "..\GraphicsEngine\Objects\IAsset.h"
#include "../GraphicsEngine/Objects/SpriteAsset.h"

#include <filesystem>
#include <memory>
#include <unordered_map>


#define WIN32_LEAN_AND_MEAN   

enum class AssetType
{
	Mesh,
	Texture,
	Animation,
	Material,
	Sprite
};

struct RegistryID
{
	AssetType Type;
	std::wstring Name;
};

class AssetManager
{
	struct AssetInfo
	{
		std::filesystem::path Path;
		std::shared_ptr<IAsset> Asset;
		bool isLoaded = false;
	};
public:
	static AssetManager& Get() { static AssetManager myInstance; return myInstance; }

	bool Init(const std::filesystem::path& aContentRootPath);

	bool RegisterAsset(const std::filesystem::path& aPath);
	bool RegisterAssetFromRelative(const std::filesystem::path& aPath);


	bool RegisterAndLoadAsset(const std::filesystem::path& aPath);
	bool RegisterAndLoadPrimitive(const std::filesystem::path& aName);

	RegistryID GetLastRegistered();

	std::filesystem::path GetContentRootDirectory();

	bool LoadAsset(const std::filesystem::path& aName);
	bool LoadAllAssets();

	void SetLastRegistered(RegistryID anId);


	template <class T>
	std::shared_ptr<T> GetAsset(const std::filesystem::path aFileName);


private:
	AssetManager() = default;

	std::filesystem::path MakeRelative(const std::filesystem::path& aPath);

	bool RegisterPrimitiveAssets();

	bool RegisterMeshAsset(const std::filesystem::path aFilePath);
	bool RegisterSpriteAsset(const std::filesystem::path aFilePath);
	bool LoadSprite(const std::wstring& aKey);
	bool RegisterAnimationAsset(const std::filesystem::path aFilePath);
	bool RegisterTextureAsset(const std::filesystem::path aFilePath);
	bool RegisterMaterialAsset(const std::filesystem::path aFilePath);
	;
private:
	std::unordered_map<std::wstring, AssetInfo> myAssetMap;
	std::filesystem::path myContentRoot;
	RegistryID myLastRegisteredKey;
};


template<class T>
std::shared_ptr<T> AssetManager::GetAsset(const std::filesystem::path aFileName)
{
	std::string name = aFileName.string();
	if (!myAssetMap.contains(aFileName.wstring()))
	{
		throw std::invalid_argument("Could not find asset with the name: " + name);
	}

	AssetInfo& info = myAssetMap.at(aFileName.wstring());
	return std::dynamic_pointer_cast<T>(info.Asset);
}


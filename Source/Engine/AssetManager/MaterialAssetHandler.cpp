#include "AssetManager.pch.h"

#include <fstream>

#include "..\External\nlohmann\json.hpp"
#include "TextureAsset.h"
#include "MaterialAsset.h"

#include "MaterialAssetHandler.h"
#include "TextureAssetHandler.h"
#include "..\GraphicsEngine\GraphicsEngine.h"



bool MaterialAssetHandler::LoadMaterialFromJSon(const std::filesystem::path& aPath, MaterialAsset& inOutAsset)
{
	std::wstring contentPath = L"Content\\" + aPath.wstring();
	nlohmann::json jsonReader;
	std::fstream file(contentPath);
	assert(file.good() && "file not found");

	if (!file.good())
	{
		return false;
	}
	file >> jsonReader;
	MaterialBufferData material = {};
	material.AlbedoColor = Vector4f(jsonReader["AlbedoColorR"], jsonReader["AlbedoColorG"], jsonReader["AlbedoColorB"], jsonReader["AlbedoColorA"]);
	material.UVTiling = Vector2f(jsonReader["UVTilingX"], jsonReader["UVTilingY"]);
	material.NormalStrength = jsonReader["NormalStrength"];
	material.Shininess = jsonReader["Shininess"];
	material.EmissionStrength = jsonReader["EmissionStrength"];
	inOutAsset.SetMaterial(material);

	std::filesystem::path albedoTextureName;
	std::filesystem::path texturePath;
	if (jsonReader.contains("AlbedoTexture"))
	{
		albedoTextureName = jsonReader["AlbedoTexture"].get<std::string>();
		texturePath = "\\Textures\\" + albedoTextureName.string();
		AssetManager::Get().RegisterAssetFromRelative(texturePath);
		AssetManager::Get().LoadAsset(albedoTextureName.stem().wstring());
	}

	std::filesystem::path normalTextureName;
	std::filesystem::path normalTexturePath;
	if (jsonReader.contains("NormalTexture"))
	{
		normalTextureName = jsonReader["NormalTexture"].get<std::string>();
		normalTexturePath = "\\Textures\\" + normalTextureName.string();
		AssetManager::Get().RegisterAssetFromRelative(normalTexturePath);
		AssetManager::Get().LoadAsset(normalTextureName.stem().wstring());
	}


	std::filesystem::path materialTextureName;
	std::filesystem::path materialTexturePath;
	if (jsonReader.contains("MaterialTexture"))
	{
		materialTextureName = jsonReader["MaterialTexture"].get<std::string>();
		materialTexturePath = "\\Textures\\" + materialTextureName.string();
		AssetManager::Get().RegisterAssetFromRelative(materialTexturePath);
		AssetManager::Get().LoadAsset(materialTextureName.stem().wstring());
	}

	std::filesystem::path fxTextureName;
	std::filesystem::path fxTexturePath;
	if (jsonReader.contains("FXTexture"))
	{
		fxTextureName = jsonReader["FXTexture"].get<std::string>();
		fxTexturePath = "\\Textures\\" + materialTextureName.string();
		AssetManager::Get().RegisterAssetFromRelative(materialTexturePath);
		AssetManager::Get().LoadAsset(materialTextureName.stem().wstring());
	}
	if (albedoTextureName != "")
	{
		inOutAsset.SetTexture(albedoTextureName.stem().wstring(), AssetManager::Get().GetAsset<TextureAsset>(albedoTextureName.stem()));
	}
	if (normalTextureName != "")
	{
		inOutAsset.SetTexture(normalTextureName.stem().wstring(), AssetManager::Get().GetAsset<TextureAsset>(normalTextureName.stem()));
	}
	if (materialTextureName != "")
	{
		inOutAsset.SetTexture(materialTextureName.stem().wstring(), AssetManager::Get().GetAsset<TextureAsset>(materialTextureName.stem()));
	}
	if (fxTextureName != "")
	{
		inOutAsset.SetTexture(fxTextureName.stem().wstring(), AssetManager::Get().GetAsset<TextureAsset>(fxTextureName.stem()));
	}
	RegistryID materialID;
	materialID.Name = inOutAsset.myName;
	materialID.Type = AssetType::Material;
	AssetManager::Get().SetLastRegistered(materialID);

    return true;
}


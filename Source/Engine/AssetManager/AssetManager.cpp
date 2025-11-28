#include "AssetManager.pch.h"
#include "AnimationAssetHandler.h"
#include "MeshAssetHandler.h"


#include "../GraphicsEngine/GraphicsEngine.h"

#include "..\External\FBXImporter/include/Importer.h"

#include "AnimationAsset.h"
#include "MeshAsset.h"
#include "SpriteAsset.h"
#include "SkeletonAsset.h"
#include "TextureAsset.h"
#include "MaterialAsset.h"
#include "Logger/Logger.h"

#include <fstream>

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(AMLog, "AM", Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(AMLog, "AM", Error);
#endif

DEFINE_LOG_CATEGORY(AMLog);



bool AssetManager::Init(const std::filesystem::path& aContentRootPath)
{
	myContentRoot = absolute(aContentRootPath);
	if (!exists(aContentRootPath) && !create_directories(aContentRootPath))
	{
		LOG(AMLog, Error, "Failed create root content folder!");
		return false;
	}
	TGA::FBX::Importer::InitImporter();
	if (!RegisterPrimitiveAssets())
	{
		LOG(AMLog, Error, "Unable to load primitive forms!");
		return false;
	}
	return true;
}

bool AssetManager::RegisterAsset(const std::filesystem::path& aPath)
{
	if (!aPath.has_extension() || !exists(aPath))
	{
		LOG(AMLog, Error, "The asset doesn't exist!");
		return false;
	}
	if (aPath.filename().extension() == ".wav") return true;
	if (aPath.extension() == ".ini") return true;
	if (aPath.filename().stem().string().ends_with("cubemap") || aPath.filename().stem().string().ends_with("Cubemap"))
	{
		return true;
	}

	std::filesystem::path assetPath = MakeRelative(aPath);
	if (!RegisterAssetFromRelative(assetPath))
	{
		return false;
	}
	return true;
}

bool AssetManager::RegisterAssetFromRelative(const std::filesystem::path& aPath)
{
	const std::string extension = aPath.extension().string();
	std::filesystem::path fileName = aPath.stem();

	if (myAssetMap.contains(fileName.wstring()))
	{
		myLastRegisteredKey.Name = fileName.wstring();
		if (extension.ends_with("fbx"))
		{
			std::basic_string name = fileName.string();
			if (name.starts_with("SM") || name.starts_with("SK"))
			{
				myLastRegisteredKey.Type = AssetType::Mesh;
			}
			else if (name.starts_with("A"))
			{
				myLastRegisteredKey.Type = AssetType::Animation;
			}
		}
		else if (fileName.string().starts_with("T") && (extension.ends_with("DDS") || extension.ends_with("dds")))
		{
			myLastRegisteredKey.Type = AssetType::Texture;
		}
		else if (fileName.string().starts_with("M") && extension.ends_with("json"))
		{
			myLastRegisteredKey.Type = AssetType::Material;
		}
		return true;
	}

	if (extension.ends_with("fbx"))
	{
		std::basic_string name = fileName.string();
		if (name.starts_with("SM") || name.starts_with("SK"))
		{
			RegisterMeshAsset(aPath);
		}
		else if (name.starts_with("A"))
		{
			RegisterAnimationAsset(aPath);
		}

		return true;
	}
	else if (fileName.string().starts_with("T") && (extension.ends_with("DDS") || extension.ends_with("dds")))
	{
		RegisterTextureAsset(aPath);
		return true;
	}
	else if (fileName.string().starts_with("M") && extension.ends_with("json"))
	{
		RegisterMaterialAsset(aPath);
		return true;
	}
	else if (fileName.string().starts_with("P") && extension.ends_with("json"))
	{
		return true;
	}
	else if (fileName.string().starts_with("sprite") && extension.ends_with("dds"))
	{
		RegisterSpriteAsset(aPath);
		LoadSprite(fileName.stem().wstring());
		myAssetMap.at(fileName.stem().wstring()).isLoaded = true;
		return true;
	}
	else
	{
		LOG(AMLog, Error, "Error: Files must be .fbx or .dds!");
		return false;
	}
	return false;
}


bool AssetManager::RegisterAndLoadAsset(const std::filesystem::path& aPath)
{
	if (!RegisterAsset(aPath))
	{
		return false;
	}
	std::filesystem::path assetPath = MakeRelative(aPath);
	const std::string extension = assetPath.extension().string();
	std::filesystem::path fileName = assetPath.stem();

	if (myAssetMap.at(fileName.wstring()).isLoaded)
	{
		return true;
	}
	if (!myAssetMap.at(fileName.wstring()).Asset->Load())
	{
		LOG(AMLog, Error, "Unable to load asset : {}", fileName.string());
		return false;
	}
	myAssetMap.at(fileName.wstring()).isLoaded = true;
	std::string message = "Added: " + fileName.string();

	return true;
}

bool AssetManager::RegisterAndLoadPrimitive(const std::filesystem::path& aName)
{
	std::shared_ptr<MeshAsset> mesh;
	AssetInfo meshInfo;
	if (aName.string() == "CubeMesh")
	{
		mesh = std::make_shared<MeshAsset>(L"CubeMesh");
		meshInfo.Path = "CubeMesh";
	}
	else if (aName.string() == "SphereMesh")
	{
		mesh = std::make_shared<MeshAsset>(L"SphereMesh");
		meshInfo.Path = "SphereMesh";
	}
	else if (aName.string() == "HorizontalPlaneMesh")
	{
		mesh = std::make_shared<MeshAsset>(L"HorizontalPlaneMesh");
		meshInfo.Path = "HorizontalPlaneMesh";
	}
	else if (aName.string() == "VerticalPlaneMesh")
	{
		mesh = std::make_shared<MeshAsset>(L"VerticalPlaneMesh");
		meshInfo.Path = "VerticalPlaneMesh";
	}
	meshInfo.Asset = mesh;
	myAssetMap.insert({ meshInfo.Path.wstring(), meshInfo });
	myLastRegisteredKey.Name = meshInfo.Path;
	myLastRegisteredKey.Type = AssetType::Mesh;
	if (!myAssetMap.at(meshInfo.Path.wstring()).Asset->Load())
	{
		LOG(AMLog, Error, "Unable to load asset : {}", meshInfo.Path.string());
		return false;
	}
	myAssetMap.at(meshInfo.Path.wstring()).isLoaded = true;
	LOG(AMLog, Log, "Mesh registered: {} ", meshInfo.Path.string());

	return true;
}

RegistryID AssetManager::GetLastRegistered()
{
	return myLastRegisteredKey;
}




std::filesystem::path AssetManager::GetContentRootDirectory()
{
	return myContentRoot;
}



std::filesystem::path AssetManager::MakeRelative(const std::filesystem::path& aPath)
{
	std::filesystem::path targetPath = relative(aPath, myContentRoot);
	targetPath = myContentRoot / targetPath;
	targetPath = weakly_canonical(targetPath);

	auto [rootEnd, nothing] = std::mismatch(
		myContentRoot.begin(),
		myContentRoot.end(),
		targetPath.begin()
	);
	if (rootEnd != myContentRoot.end())
	{
		throw std::invalid_argument("Path is not below the root!");
	}
	if (aPath.is_absolute())
	{
		return relative(aPath, myContentRoot);
	}
	return aPath;
}

bool AssetManager::RegisterPrimitiveAssets()
{
	std::shared_ptr<MeshAsset> ground = std::make_shared<MeshAsset>(L"GroundMesh");
	AssetInfo groundInfo;
	groundInfo.Path = "GroundMesh";
	groundInfo.Asset = ground;
	myAssetMap.insert({ L"GroundMesh", groundInfo });
	myLastRegisteredKey.Name = groundInfo.Path;
	myLastRegisteredKey.Type = AssetType::Mesh;

	std::shared_ptr<MeshAsset> cube = std::make_shared<MeshAsset>(L"CubeMesh");
	AssetInfo cubeInfo;
	cubeInfo.Path = "CubeMesh";
	cubeInfo.Asset = cube;
	myAssetMap.insert({ L"CubeMesh", cubeInfo });
	myLastRegisteredKey.Name = groundInfo.Path;
	myLastRegisteredKey.Type = AssetType::Mesh;

	std::shared_ptr<MeshAsset> sphere = std::make_shared<MeshAsset>(L"SphereMesh");
	AssetInfo sphereInfo;
	sphereInfo.Path = "SphereMesh";
	sphereInfo.Asset = sphere;
	myAssetMap.insert({ L"SphereMesh", sphereInfo });
	myLastRegisteredKey.Name = sphereInfo.Path;
	myLastRegisteredKey.Type = AssetType::Mesh;

	std::shared_ptr<MeshAsset> verticalPlane = std::make_shared<MeshAsset>(L"VerticalPlaneMesh");
	AssetInfo verticalPlaneInfo;
	verticalPlaneInfo.Path = "VerticalPlaneMesh";
	verticalPlaneInfo.Asset = verticalPlane;
	myAssetMap.insert({ L"VerticalPlaneMesh", verticalPlaneInfo });
	myLastRegisteredKey.Name = verticalPlaneInfo.Path;
	myLastRegisteredKey.Type = AssetType::Mesh;

	std::shared_ptr<MeshAsset> horizontalPlane = std::make_shared<MeshAsset>(L"HorizontalPlaneMesh");
	AssetInfo horizontalPlaneInfo;
	horizontalPlaneInfo.Path = "HorizontalPlaneMesh";
	horizontalPlaneInfo.Asset = horizontalPlane;
	myAssetMap.insert({ L"HorizontalPlaneMesh", horizontalPlaneInfo });
	myLastRegisteredKey.Name = horizontalPlaneInfo.Path;
	myLastRegisteredKey.Type = AssetType::Mesh;
	return true;
}

bool AssetManager::RegisterMeshAsset(const std::filesystem::path aFilePath)
{
	std::wstring fileName = aFilePath.stem();
	if (myAssetMap.contains(fileName))
	{
		myLastRegisteredKey.Name = fileName;
		myLastRegisteredKey.Type = AssetType::Mesh;
		return true;
	}
	std::shared_ptr<MeshAsset> mesh = std::make_shared<MeshAsset>(aFilePath);

	AssetInfo meshInfo;
	meshInfo.Path = aFilePath;
	meshInfo.Asset = mesh;
	myAssetMap.insert({ fileName, meshInfo });
	myLastRegisteredKey.Name = fileName;
	myLastRegisteredKey.Type = AssetType::Mesh;
	LOG(AMLog, Log, "Mesh registered: {}", aFilePath.stem().string());
	return false;
}

bool AssetManager::RegisterSpriteAsset(const std::filesystem::path aFilePath)
{
	std::wstring fileName = aFilePath.stem();
	if (myAssetMap.contains(fileName))
	{
		myLastRegisteredKey.Name = fileName;
		myLastRegisteredKey.Type = AssetType::Mesh;
		return true;
	}
	std::shared_ptr<SpriteAsset> sprite = std::make_shared<SpriteAsset>(aFilePath);

	AssetInfo spriteInfo;
	spriteInfo.Path = aFilePath;
	spriteInfo.Asset = sprite;
	myAssetMap.insert({ fileName, spriteInfo });
	myLastRegisteredKey.Name = fileName;
	myLastRegisteredKey.Type = AssetType::Sprite;
	LOG(AMLog, Log, "Sprite registered: {}", aFilePath.stem().string());
	return false;
}

bool AssetManager::LoadSprite(const std::wstring& aKey)
{
	if (!myAssetMap.contains(aKey))
	{
		return false;
	}
	AssetInfo& asset = myAssetMap.at(aKey);
	std::shared_ptr<SpriteAsset> sprite = std::dynamic_pointer_cast<SpriteAsset>(asset.Asset);
	sprite->SetTexture(std::make_shared<TextureAsset>());
	if (!GraphicsEngine::Get().LoadTexture(myContentRoot / asset.Path, *sprite->GetTexture()))
	{
		return false;
	}
	asset.isLoaded = true;
	CU::Vector2f pos = { 500.5f, 500.5f };
	CU::Vector2f size = { 100.5f, 100.5f };
	sprite->InitAbsolute(pos, size, sprite->GetTexture());
	return true;
}



bool AssetManager::RegisterAnimationAsset(const std::filesystem::path aFilePath)
{
	std::filesystem::path fileName = aFilePath.stem();
	if (myAssetMap.contains(fileName))
	{
		myLastRegisteredKey.Name = fileName;
		myLastRegisteredKey.Type = AssetType::Animation;
		return true;
	}
	std::shared_ptr<AnimationAsset> animation = std::make_shared<AnimationAsset>(aFilePath);
	AssetInfo animInfo;
	animInfo.Path = aFilePath;
	animInfo.Asset = animation;
	myAssetMap.insert({ fileName, animInfo });
	myLastRegisteredKey.Name = fileName;
	myLastRegisteredKey.Type = AssetType::Animation;
	LOG(AMLog, Log, "Animation registered: {}", aFilePath.stem().string());
	return true;
}

bool AssetManager::RegisterTextureAsset(const std::filesystem::path aFilePath)
{
	//Textures, Cubemaps, Normal maps & MaterialTextures!!
	std::wstring fileName = aFilePath.stem();
	if (myAssetMap.contains(fileName))
	{
		myLastRegisteredKey.Name = fileName;
		myLastRegisteredKey.Type = AssetType::Texture;
		return true;
	}
	std::shared_ptr<TextureAsset> texture = std::make_shared<TextureAsset>(aFilePath);
	AssetInfo texInfo;
	texInfo.Path = aFilePath;
	texInfo.Asset = texture;
	myAssetMap.insert({ fileName, texInfo });
	myLastRegisteredKey.Name = fileName;
	myLastRegisteredKey.Type = AssetType::Texture;
	LOG(AMLog, Log, "Texture registered: {}", aFilePath.stem().string());
	return true;
}

bool AssetManager::RegisterMaterialAsset(const std::filesystem::path aFilePath)
{
	std::wstring fileName = aFilePath.stem();
	if (myAssetMap.contains(fileName))
	{
		myLastRegisteredKey.Name = fileName;
		myLastRegisteredKey.Type = AssetType::Material;
		return true;
	}
	std::shared_ptr<MaterialAsset> material = std::make_shared<MaterialAsset>(aFilePath);
	AssetInfo matInfo;
	matInfo.Path = aFilePath;
	matInfo.Asset = material;
	myAssetMap.insert({ fileName, matInfo });
	myLastRegisteredKey.Name = fileName;
	myLastRegisteredKey.Type = AssetType::Material;
	LOG(AMLog, Log, "Material registered: {}", aFilePath.stem().string());
	return true;
}

bool AssetManager::LoadAsset(const std::filesystem::path& aName)
{
	if (myAssetMap.contains(aName.wstring()))
	{
		if (myAssetMap.at(aName.wstring()).isLoaded)
		{
			return true;
		}
		myAssetMap.at(aName.wstring()).Asset->Load();
		myAssetMap.at(aName.wstring()).isLoaded = true;
		return true;
	}
	return false;
}

bool AssetManager::LoadAllAssets()
{
	for (auto& c : myAssetMap)
	{
		if (c.second.Asset.get() != nullptr)
		{
			if (!c.second.Asset->Load())
			{
				std::filesystem::path name = c.first;
				LOG(AMLog, Error, "Unable to load : {}", name.string());
				return false;
			}
			c.second.isLoaded = true;
		}
	}
	return true;
}

void AssetManager::SetLastRegistered(RegistryID anId)
{
	myLastRegisteredKey = anId;
}


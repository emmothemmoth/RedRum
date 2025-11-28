#include "GraphicsEngine.pch.h"
#include "SkeletonAsset.h"

#include "../../AssetManager/MeshAssetHandler.h"

SkeletonAsset::SkeletonAsset()
{
}

SkeletonAsset::SkeletonAsset(const std::filesystem::path& aPath)
{
	myPath = aPath;
	myName = aPath.stem().wstring();
}

bool SkeletonAsset::Load()
{
	if (MeshAssetHandler::Get().LoadSkeletonFromFBX(myPath, *this))
	{
		return true;
	}
	return false;
}

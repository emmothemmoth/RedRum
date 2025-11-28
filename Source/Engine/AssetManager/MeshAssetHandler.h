#pragma once
#include "MeshAsset.h"
#include "SkeletonAsset.h"
#include <filesystem>

class MeshAssetHandler
{
public:
	static MeshAssetHandler& Get() { static MeshAssetHandler myInstance; return myInstance; }

	void InitGround(MeshAsset& inOutAsset);
	void InitCube(MeshAsset& inOutAsset);
	void InitSphere(MeshAsset& inOutAsset);
	void InitHorizontalPlane(MeshAsset& inOutAsset);
	void InitVerticalPlane(MeshAsset& inOutAsset);

	bool LoadMeshFromFBX(const std::filesystem::path& aPath, MeshAsset& inOutAsset);
	bool LoadSkeletonFromFBX(const std::filesystem::path& aPath, SkeletonAsset& inOutAsset);

private:
};

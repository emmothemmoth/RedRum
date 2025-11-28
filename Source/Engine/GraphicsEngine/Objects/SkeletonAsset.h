#pragma once

#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>

#include "Socket.h"
#include "../Utilities/CommonUtilities/Matrix4x4.hpp"
#include "IAsset.h"


struct SkeletonAsset : public IAsset
{
	struct Bone
	{
		std::string name;
		unsigned int parentIndex;
		CommonUtilities::Matrix4x4<float> bindPoseInverse;
		std::vector<unsigned int> children;
	};
	std::wstring myName;
	std::wstring myPath;
	SkeletonAsset();
	SkeletonAsset(const std::filesystem::path& aPath);
	SkeletonAsset(const SkeletonAsset& aSkeleton) = default;
	bool Load() override;
	const Bone* GetRoot() const { if (!bones.empty()) { return &bones[0]; } return nullptr; }
	std::vector<Bone> bones;
	std::unordered_map<std::string, Socket> sockets;
	std::unordered_map<std::string, size_t> boneNameToIndex;
};


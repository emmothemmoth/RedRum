#pragma once

#include <filesystem>

struct AnimationAsset;

class AnimationAssetHandler
{
public:
	static AnimationAssetHandler& Get() { static AnimationAssetHandler myInstance; return myInstance; }

	bool LoadAnimationFromFBX(const std::filesystem::path& aPath, AnimationAsset& inOutAsset);

private:

};

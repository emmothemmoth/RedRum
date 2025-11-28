#include "GraphicsEngine.pch.h"
#include "AnimationAsset.h"

#include "../../AssetManager/AnimationAssetHandler.h"

AnimationAsset::AnimationAsset(const std::filesystem::path& aPath)
{
	myPath = aPath;
    myName = aPath.stem().string();
}

void AnimationAsset::PushFrame(const Frame& aFrame)
{
    myFrames.push_back(aFrame);
}

void AnimationAsset::PushEvent(const std::string& anEventName)
{
    myEvents.push_back(anEventName);
}

void AnimationAsset::SetName(const std::string& aName)
{
    myName = aName;
}

void AnimationAsset::SetDuration(const double& aDuration)
{
    myDuration = aDuration;
}

void AnimationAsset::SetLength(const unsigned int& aFrameCount)
{
    myLength = aFrameCount;
}

void AnimationAsset::SetFPS(const float& anFPS)
{
    myFPS = anFPS;
}

std::vector<Frame> AnimationAsset::GetFrames() const
{
    return myFrames;
}

float AnimationAsset::GetFPS() const
{
    return myFPS;
}

unsigned int AnimationAsset::GetLength() const
{
    return myLength;
}

bool AnimationAsset::Load()
{
    if (AnimationAssetHandler::Get().LoadAnimationFromFBX(myPath, *this))
    {
        return true;
    }
    return false;
}

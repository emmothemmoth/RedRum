#pragma once
#include "AnimationState.h"

#include "..\GraphicsEngine\Objects\SkeletonAsset.h"

#include <memory>
#include <vector>


struct AnimationAsset;


class AnimationPlayer
{
public:
	AnimationPlayer();
	void SetFPS(const float& anFPS);
	//void Play(const std::vector<SkeletonAsset::Bone>& aSkeletonBones, const std::weak_ptr<AnimationAsset>& anAnimation, const float& aDeltaTime, std::vector<CommonUtilities::Matrix4x4<float>>& someTransforms);
	void Play(const std::vector<SkeletonAsset::Bone>& aSkeletonBones, const std::shared_ptr<AnimationAsset>& anAnimation, const float& aDeltaTime, std::vector<CommonUtilities::Matrix4x4<float>>& someTransforms);
private:
	void UpdateAnimation(const std::vector<SkeletonAsset::Bone>& someBones, const std::unordered_map<std::string, CommonUtilities::Matrix4x4<float>> aLocalTransforms, std::vector<CommonUtilities::Matrix4x4<float>>& outMatrices);
	float myTimer;
	float myLerpValue;
	unsigned int myCurrentFrame;
	float myFrameTime;
};

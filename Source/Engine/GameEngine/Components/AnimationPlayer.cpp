#include "GameEngine.pch.h"

#include "AnimationPlayer.h"

#include "..\GraphicsEngine\Objects\AnimationAsset.h"

#include <cmath>

AnimationPlayer::AnimationPlayer()
	:myCurrentFrame(0)
	,myTimer(0)
	,myLerpValue(0)
{
	myFrameTime = 0.0f;

}
void AnimationPlayer::SetFPS(const float& anFPS)
{
	myFrameTime = anFPS * 0.001f;
}
void AnimationPlayer::Play(const std::vector<SkeletonAsset::Bone>& aSkeletonBones, const std::shared_ptr<AnimationAsset>& anAnimation, const float& aDeltaTime, std::vector<CommonUtilities::Matrix4x4<float>>& someTransforms)
{
	myTimer += aDeltaTime;
	myLerpValue = myFrameTime / myTimer;
	if (myTimer >= myFrameTime)
	{
		myTimer = 0.0f;
		++myCurrentFrame;
		if (myCurrentFrame >= anAnimation->GetLength())
		{
			myCurrentFrame = 0;
		}
		UpdateAnimation(aSkeletonBones, anAnimation->GetFrames()[myCurrentFrame].LocalTransforms, someTransforms);
	}
}
void AnimationPlayer::UpdateAnimation(const std::vector<SkeletonAsset::Bone>& someBones, const std::unordered_map<std::string, CommonUtilities::Matrix4x4<float>> aLocalTransforms, std::vector<CommonUtilities::Matrix4x4<float>>& outMatrices)
{
	std::vector<CommonUtilities::Matrix4x4<float>> localTransform(someBones.size());
	std::vector<CommonUtilities::Matrix4x4<float>> modelTransform(someBones.size());

	for (int index = 0; index < someBones.size(); ++index)
	{
		localTransform[index] = aLocalTransforms.at(someBones[index].name);
	}
	modelTransform[0] = localTransform[0];

	for (int index = 1; index < someBones.size(); ++index)
	{
		int parent = someBones[index].parentIndex;
		if (parent < someBones.size() || parent >= 0)
		{
			modelTransform[index] = modelTransform[parent] * localTransform[index];
		}
	}

	for (int index = 0; index < someBones.size(); ++index)
	{
		CommonUtilities::Matrix4x4<float> bindTranspose = CommonUtilities::Matrix4x4<float>::Transpose(someBones[index].bindPoseInverse);
		outMatrices[index] = modelTransform[index] * bindTranspose;
	}
}

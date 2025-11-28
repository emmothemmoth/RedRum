#pragma once

#include "Component.h"
#include "AnimationState.h"
#include "AnimationPlayer.h"

#include <memory>
#include <unordered_map>

struct AnimationAsset;
struct SkeletonAsset;

class AnimationComponent : public Component
{
public:
	AnimationComponent() = delete;
	//AnimationComponent(GameObject& aParent, const std::weak_ptr<SkeletonAsset>& aSkeletonAsset);
	AnimationComponent(GameObject& aParent, const SkeletonAsset& aSkeletonAsset);
	~AnimationComponent();

	void Update(const float aDeltaTime) override;

	//void AddAnimation(const AnimationState& anAnimationState, const std::weak_ptr<AnimationAsset> anAnimation);
	void AddAnimation(const AnimationState& anAnimationState, const std::shared_ptr<AnimationAsset> anAnimation);
	void AddAndSetAnimation(const std::shared_ptr<AnimationAsset> anAnimation);
	void SetAnimationState(const AnimationState& anAnimationState);
	std::vector<CommonUtilities::Matrix4x4<float>>& GetBoneTransforms();
	//std::weak_ptr<AnimationAsset> GetAnimation();
	std::shared_ptr<AnimationAsset> GetAnimation();

	

private:
	//std::weak_ptr<SkeletonAsset> mySkeleton;
	SkeletonAsset mySkeleton;
	AnimationState myState;
	//std::unordered_map < AnimationState,  std::weak_ptr<AnimationAsset>> myAnimationMap;
	std::unordered_map < AnimationState,  std::shared_ptr<AnimationAsset>> myAnimationMap;
	std::vector<CommonUtilities::Matrix4x4<float>> myCurrentBoneTransforms;

	AnimationPlayer myAnimationPlayer;
};
#include "GameEngine.pch.h"
#include "AnimationComponent.h"
#include "../GraphicsEngine/Objects/AnimationAsset.h"
#include "../GraphicsEngine/Objects/SkeletonAsset.h"
#include "../../AssetManager/AssetManager.h"
#include "GameObject.h"

AnimationComponent::AnimationComponent(GameObject& aParent, const SkeletonAsset& aSkeletonAsset)
	: Component(aParent),
	mySkeleton(aSkeletonAsset)
{
	myID = ComponentID::Animation;
	myState = AnimationState::Walking;
	myCurrentBoneTransforms.resize(mySkeleton.bones.size());
}

AnimationComponent::~AnimationComponent()
{
}

void AnimationComponent::Update(const float aDeltaTime)
{
	myAnimationPlayer.Play(mySkeleton.bones, GetAnimation(), aDeltaTime, myCurrentBoneTransforms);
}

void AnimationComponent::AddAnimation(const AnimationState& anAnimationState, const std::shared_ptr<AnimationAsset> anAnimation)
{
	if (myAnimationMap.contains(anAnimationState))
	{
		myAnimationMap.at(anAnimationState) = anAnimation;
	}
	else
	{
		myAnimationMap.insert({ anAnimationState, anAnimation });
	}
}

void AnimationComponent::AddAndSetAnimation(const std::shared_ptr<AnimationAsset> anAnimation)
{
	if (myAnimationMap.contains(AnimationState::Idle))
	{
		myAnimationMap.at(AnimationState::Idle) = anAnimation;
	}
	else
	{
		myAnimationMap.insert({ AnimationState::Idle, anAnimation });
	}
	SetAnimationState(AnimationState::Idle);
}


void AnimationComponent::SetAnimationState(const AnimationState& anAnimationState)
{
	myState = anAnimationState;
	std::shared_ptr<AnimationAsset> animation(myAnimationMap.at(myState));
	myAnimationPlayer.SetFPS(animation->GetFPS());
}

std::vector<CommonUtilities::Matrix4x4<float>>& AnimationComponent::GetBoneTransforms()
{
	return myCurrentBoneTransforms;
}

std::shared_ptr<AnimationAsset> AnimationComponent::GetAnimation()
{
	return myAnimationMap.at(myState);
}

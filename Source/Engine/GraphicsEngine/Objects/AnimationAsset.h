#pragma once
#include "IAsset.h"
#include "Frame.h"

#include <string>
#include <filesystem>
#include <vector>

struct AnimationAsset : public IAsset
{
public:
	AnimationAsset() = default;
	AnimationAsset(const AnimationAsset& anAnimation) = default;
	AnimationAsset(const std::filesystem::path& aPath);
	~AnimationAsset() = default;

	void PushFrame(const Frame& aFrame);
	void PushEvent(const std::string& anEventName);
	void SetName(const std::string& aName);
	void SetDuration(const double& aDuration);
	void SetLength(const unsigned int& aFrameCount);
	void SetFPS(const float& anFPS);

	std::vector<Frame> GetFrames() const;
	float GetFPS() const;
	unsigned int GetLength() const;
	bool Load() override;

private:
	std::wstring myPath;
	std::string myName;
	std::vector<Frame> myFrames;
	std::vector<std::string> myEvents;
	double myDuration;
	unsigned int myLength;
	float myFPS;
	
};


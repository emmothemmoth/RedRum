#pragma once
#include "Component.h"
#include <filesystem>

#include "../AudioEngine/RoomSimulator/EmitterSettings.h"
#include "../AudioEngine/RoomSimulator/VisualRayPath.h"

#include "MainSingleton.h"

class DebugLineObject;
class AudioEngine;

struct RayAnimationTracker
{
	VisualRayPath PathData;
	int CurrentBounceIndex = 0;
	float CurrentBounceProgress = 0.0f; 
	float Speed = 2500.0f;
};

class AudioSourceComponent : public Component
{
public:
	AudioSourceComponent(GameObject& aParent);
	~AudioSourceComponent();

	void Init(const std::filesystem::path& aSourceFile);
	void Update(const float aDeltaTime) override;
	void Render() override;

	void Play();
	void Pause();
	void Stop();

	bool IsPlayable() const { return myIsPlayable; }

	const std::filesystem::path& GetAudioSourceName() const { return myFilePath; }
	EmitterSettings& GetSettings() { return mySettings; }

	void MarkDirty() { myIsDirty = true; }

private:
	void ClearRays();
	void ReceiveScoutRays(EmitterHandle aHandle, std::vector<VisualRayPath> somePaths);
private:
	std::vector<RayAnimationTracker> myAnimators;
	std::shared_ptr<DebugLineObject> myDebugLines;
	std::filesystem::path myFilePath;
	EmitterSettings mySettings;
	uint32_t mySourceHandle = UINT32_MAX;
	uint32_t myEmitterHandle = UINT32_MAX;
	DelegateHandle myVisualizeHandle;
	DelegateHandle myScoutHandle;
	float myLineAlpha = 1.0f;
	bool myIsAnimatingRays = false;
	bool myAnimationDone = false;
	bool myIsPlayable = false;
	bool myIsDirty = false;
};
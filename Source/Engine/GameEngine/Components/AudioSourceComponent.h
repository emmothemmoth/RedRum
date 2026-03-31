#pragma once
#include "Component.h"
#include <filesystem>

#include "../AudioEngine/RoomSimulator/EmitterSettings.h"
#include "../../GraphicsEngine/Objects/DebugLineObject.h"

#include "MainSingleton.h"

class AudioEngine;

struct VisualRay
{
	CU::Vector3f Origin;
	CU::Vector3f Direction;
	float MaxDistance = 1000.0f; // Where it hits a wall (you'll get this from a physics raycast later)
	float CurrentDistance = 0.0f;
	float Speed = 500.0f; // Units per second
	bool IsFinished = false;
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

	void StartVisualRays();
private:
	std::vector<VisualRay> myVisualRays;
	std::shared_ptr<DebugLineObject> myDebugLines;
	bool myIsAnimatingRays = false;
	std::filesystem::path myFilePath;
	EmitterSettings mySettings;
	uint32_t mySourceHandle = UINT32_MAX;
	uint32_t myEmitterHandle = UINT32_MAX;
	DelegateHandle myVisualizeHandle;
	bool myIsPlayable = false;
	bool myIsDirty = false;
};
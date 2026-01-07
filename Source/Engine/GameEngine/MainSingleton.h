#pragma once

#include "Renderer.h"
#include "CommonUtilities/InputMapper.h"
#include "../AudioEngine/AudioEngine.h"

class MainSingleton
{
public:
	MainSingleton() = default;
	~MainSingleton() = default;

	static MainSingleton& Get();

	Renderer& GetRenderer() { return  myRenderer; }
	CommonUtilities::InputMapper& GetInputMapper() { return myInputMapper; }
	AudioEngine& GetAudioEngine();

private:
	Renderer myRenderer;
	CommonUtilities::InputMapper myInputMapper;
	AudioEngine myAudioEngine;
};


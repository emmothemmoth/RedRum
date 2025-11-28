#pragma once

#include "Renderer.h"
#include "CommonUtilities/InputMapper.h"

class MainSingleton
{
public:
	MainSingleton() = default;
	~MainSingleton() = default;

	static MainSingleton& Get();

	Renderer& GetRenderer() { return  myRenderer; }
	CommonUtilities::InputMapper& GetInputMapper() { return myInputMapper; }

private:
	Renderer myRenderer;
	CommonUtilities::InputMapper myInputMapper;
};


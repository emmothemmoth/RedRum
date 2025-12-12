#include "Editor.pch.h"
#include "Editor.h"

#include "../Engine/GraphicsEngine/GraphicsEngine.h"
#include "../Engine/GraphicsEngine/GraphicsCommands.h"
#include "../Engine/GraphicsEngine/PipelineStates.h"
#include "../Engine/GraphicsEngine/Buffers/FrameBuffer.h"

#include "../Engine/AssetManager/AssetManager.h"
#include "CommonUtilities/UtilityFunctions.hpp"

#include "Scene.h"
#include "MainSingleton.h"

#include "Windows.h"
#include "shellapi.h"
#include <iostream>

Editor::~Editor()
{
}

void Editor::Init()
{
	const std::filesystem::path startDir = std::filesystem::current_path().parent_path();
	const std::filesystem::path contentDir = startDir / "Bin" / "Content";
	myScene = std::make_shared<Scene>();
	myScene->Init();
	SIZE windowSize = GraphicsEngine::Get().GetWindowSize();
	DragAcceptFiles(GraphicsEngine::Get().GetWindowHandle(), true); //Accept drag&drop to window

	myGUI.Init(GraphicsEngine::Get().GetWindowHandle(), GraphicsEngine::Get().GetRHI()->GetDevice().Get(), GraphicsEngine::Get().GetRHI()->GetContext().Get(), GraphicsEngine::Get().GetRenderSize());

	for (const auto& audioFile : std::filesystem::recursive_directory_iterator(contentDir / "Audio"))
	{
		if (audioFile.path().has_filename() && audioFile.path().has_extension())
		{
		}
	}
	MapInputs();
	myLoadingDone = true;
}

void Editor::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	myIsRunning = true;
	while (myIsRunning)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_DROPFILES)
			{
				TCHAR filePath[MAX_PATH];
				HDROP dropHandle = (HDROP)msg.wParam;
				DragQueryFileW(dropHandle, 0, filePath, MAX_PATH);
				std::filesystem::path path = filePath;

			}

			if (msg.message == WM_QUIT)
			{
				myIsRunning = false;
#ifndef _RETAIL
				myGUI.ShutDown();
#endif
				return;
			}
		}
		MainSingleton::Get().GetRenderer().RenderFrame();
		GraphicsEngine::Get().ChangeRenderTarget(GraphicsEngine::Get().GetRHI()->GetBackBuffer());
		myGUI.Update();
		myGUI.Render();
		myScene->PresentScene();
	}
}

void Editor::ShutDown()
{
	myIsRunning = false;
}

bool Editor::LoadScene(const std::filesystem::path& aLevel)
{
	myLoadingDone.store(false, std::memory_order::release);
	myScene->LoadScene(aLevel.empty() ? "Content\\Levels\\AssetGym.json" : aLevel.string());
	myLoadingDone.store(true, std::memory_order_release);
	return true;
}

void Editor::UnloadScene()
{
}

bool Editor::CreateNewScene(const std::filesystem::path& aSceneName)
{
	aSceneName;
	return false;
}

void Editor::UpdateLoop()
{
	while (myLoadingDone.load(std::memory_order_acquire) == false)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	//For fixed update timing
	constexpr float fps = 120.f;
	constexpr float fixedDelta = (1000.0f / fps);

	auto startTime = std::chrono::high_resolution_clock::now();
	double fractionalTime = 0;
	unsigned warmupFrames = 5; // just to not update the first few frames where delta is very very low

	while (myIsRunning)
	{
		const auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> frametime = currentTime - startTime;

		const auto passedMS = frametime.count() + fractionalTime;
		if (static_cast<float>(passedMS) <= fixedDelta)
		{
			continue;
		}
		startTime = currentTime;
		fractionalTime = CU::Max(static_cast<float>(passedMS) - fixedDelta, 0.0f);

		CU::Timer::Get().Update();

		if (warmupFrames > 0)
		{
			warmupFrames--;
			continue;
		}
		MainSingleton::Get().GetInputMapper().Refresh();

		myScene->UpdateScene(CU::Timer::Get().GetDeltaTime());
		myScene->RenderScene();
	}
}

void Editor::MapInputs()
{
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Move_Left, ActionEventID::CameraMove_Left);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Move_Forward, ActionEventID::CameraMove_Forward);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Move_Backwards, ActionEventID::CameraMove_Backwards);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Move_Right, ActionEventID::CameraMove_Right);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Fly_Up, ActionEventID::CameraMove_Up);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Fly_Down, ActionEventID::CameraMove_Down);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Rotation, ActionEventID::CameraRotation);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::ToggleDirectionalLight, ActionEventID::Toggle_DirectionalLight);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::TogglePointLight, ActionEventID::Toggle_PointLights);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::ToggleSpotLight, ActionEventID::Toggle_SpotLights);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::ChangeTonemap, ActionEventID::ChangeTonemap);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::ExposureUp, ActionEventID::ExposureUp);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::ExposureDown, ActionEventID::ExposureDown);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::ChangeLuminanceMode, ActionEventID::ChangeLuminanceMode);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Toggle_SSAO, ActionEventID::Toggle_SSAO);
}



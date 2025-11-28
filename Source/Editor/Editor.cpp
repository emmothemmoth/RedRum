#include "Editor.pch.h"
#include "Editor.h"

#include "Scene.h"
#include "MainSingleton.h"
#include "../Engine/GraphicsEngine/GraphicsEngine.h"
#include "../Engine/GraphicsEngine/GraphicsCommands.h"
#include "../Engine/GraphicsEngine/PipelineStates.h"
#include "../Engine/GraphicsEngine/Buffers/FrameBuffer.h"

#include "../Engine/AssetManager/AssetManager.h"
#include "CommonUtilities/UtilityFunctions.hpp"

#include "Windows.h"
#include "shellapi.h"

Editor::~Editor()
{
}

void Editor::Init()
{
	const std::filesystem::path startDir = std::filesystem::current_path().parent_path();
	const std::filesystem::path contentDir = startDir / "Bin" / "Content";
	AssetManager::Get().Init(contentDir);
	AssetManager::Get().LoadAllAssets();
	myScene = std::make_shared<Scene>();
	myScene->Init();
	for (const auto& file : std::filesystem::recursive_directory_iterator(AssetManager::Get().GetContentRootDirectory()))
	{
		if (file.path().has_filename() && file.path().has_extension())
		{
			if (AssetManager::Get().RegisterAsset(file.path()))
			{

			}
		}
	}
	SIZE windowSize = GraphicsEngine::Get().GetWindowSize();
	DragAcceptFiles(GraphicsEngine::Get().GetWindowHandle(), true); //Accept drag&drop to window

	myGUI.Init(GraphicsEngine::Get().GetWindowHandle(), GraphicsEngine::Get().GetRHI()->GetDevice().Get(), GraphicsEngine::Get().GetRHI()->GetContext().Get(), GraphicsEngine::Get().GetWindowSizeAsVector());

	for (const auto& audioFile : std::filesystem::recursive_directory_iterator(contentDir / "AudioFiles"))
	{
		if (audioFile.path().has_filename() && audioFile.path().has_extension())
		{
		}
	}
	
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
	myLoadingDone = false;
	myScene->LoadScene(aLevel.empty() ? "Content\\Levels\\AssetGym.json" : aLevel.string());
	myLoadingDone = true;
	return true;
}

void GUI::UnloadScene()
{
}

bool GUI::CreateNewScene(const std::filesystem::path& aSceneName)
{
	return false;
}

void Editor::UpdateLoop()
{
	do
	{
		std::this_thread::yield();
	} while (!myLoadingDone);

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

		myScene->UpdateScene(myTimer.GetDeltaTime());
		myScene->RenderScene();
	}
}



#pragma once
#include "GUI.h"
#include "CommonUtilities/Timer.h"

#include <atomic>


class Scene;
using FOnExternalFileDropped = MulticastDelegate<const std::filesystem::path&>;
class Editor
{
public:
	Editor() = default;
	~Editor();

	void Init();

	void Run();

	void ShutDown();

	bool LoadScene(const std::filesystem::path& aLevel = "");
	void UnloadScene();
	bool CreateNewScene(const std::filesystem::path& aSceneName);

	void UpdateLoop();

	void ResolutionChanged(CU::Vector2U aResolution);

private:
	void MapInputs();
	void RegisterCallbacks();

public:
#pragma region Delegates
	FOnExternalFileDropped OnExternalFileDropped;

#pragma endregion Delegates
private:
	GUI myGUI;
	std::shared_ptr<Scene> myScene;
	std::atomic<bool> myLoadingDone = false;
	std::atomic<bool> myIsRunning = false;
};


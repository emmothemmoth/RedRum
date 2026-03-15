#pragma once
#include "GUI.h"
#include "CommonUtilities/Timer.h"

#include <atomic>


class Scene;

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
private:
	GUI myGUI;
	std::shared_ptr<Scene> myScene;
	std::atomic<bool> myLoadingDone = false;
	std::atomic<bool> myIsRunning = false;
};


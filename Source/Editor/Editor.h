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

	void UpdateLoop();

private:

private:
	GUI myGUI;
	CU::Timer myTimer;
	std::shared_ptr<Scene> myScene;
	std::atomic<bool> myLoadingDone = false;
	std::atomic<bool> myIsRunning = false;
};


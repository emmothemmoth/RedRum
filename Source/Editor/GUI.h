#pragma once

#include "GuiCmd.h"

#include "../Engine/Utilities/CommonUtilities/Vector2.hpp"
#include "../Engine/External/DearImGui/imgui.h"

#include "Windows.h"
#include <memory>
#include <vector>
#include <array>
#include <filesystem>
#include <unordered_map>
#include <queue>


struct ID3D11Device;
struct ID3D11DeviceContext;

class GUI
{
public:
	GUI() = default;
	~GUI() = default;

	void Init(HWND aWindowHandle, ID3D11Device* aDX11Device, ID3D11DeviceContext* aDX11Context, const CU::Vector2f& aResolution);

	bool LoadScene(const std::filesystem::path& aSceneName);
	void UnloadScene();
	bool CreateNewScene(const std::filesystem::path& aSceneName);

	void Update();
			
	void Render();

	void ShutDown();

private:
	CU::Vector2f myResolution;
};

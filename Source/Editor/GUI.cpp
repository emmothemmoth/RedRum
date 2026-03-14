#include "Editor.pch.h"
#include "GUI.h"
#include "GuiCmdInclude.h"

#include "../Engine/External/DearImGui/imgui_impl_win32.h"
#include "../Engine/External/DearImGui/imgui_impl_dx11.h"
#include "../Engine/GraphicsEngine/Objects/TextureAsset.h"
#include "../Engine/GraphicsEngine/Objects/SpriteAsset.h"


#include "../Engine/Utilities/CommonUtilities/Input.h"
#include "../Engine/Utilities/CommonUtilities/Input.h"
#include "../Engine/Utilities/CommonUtilities/Timer.h"
#include <fstream>
#include <windows.h>
#include <commdlg.h>
#include <tchar.h>
#include <string>
#include <algorithm>




void GUI::Init(HWND aWindowHandle, ID3D11Device* aDX11Device, ID3D11DeviceContext* aDX11Context, const CU::Vector2f& aResolution)
{
	myResolution = aResolution;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(aWindowHandle);
	ImGui_ImplDX11_Init(aDX11Device, aDX11Context);
}


void GUI::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//update editortools and other stuff
	//fetch info from scene about selected objects

	ImGui::Render();
	CaptureDrawData(myWriteFrame);
}

void GUI::CaptureDrawData(GuiFrameData& outFrame)
{
	//ImDrawData* src = ImGui::GetDrawData();
    //if (!src || src->CmdListsCount == 0)
    //    return;
	//
    //outFrame.Clear();
	//
    //// Copy draw lists
    //outFrame.CmdLists.reserve(src->CmdListsCount);
	//
    //for (int i = 0; i < src->CmdListsCount; i++)
    //{
    //    ImDrawList* cloned = src->CmdLists[i]->CloneOutput();
    //    outFrame.CmdLists.push_back(cloned);
    //}
	//
    //// Copy draw data struct
    //outFrame.DrawData = *src;
	//
    //// Replace command list pointers with our copies
    //outFrame.DrawData.CmdLists = outFrame.CmdLists.data();
    //outFrame.DrawData.CmdListsCount = (int)outFrame.CmdLists.size(); // TODO:
}

void GUI::Render(const GuiFrameData& aFrame)
{
	ImGui_ImplDX11_RenderDrawData((ImDrawData*)&aFrame.DrawData);
}


void GUI::ShutDown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}




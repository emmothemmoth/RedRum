#include "Editor.pch.h"
#include "GUI.h"
#include "GuiCmdInclude.h"

#include "../Engine/External/DearImGui/imgui_impl_win32.h"
#include "../Engine/External/DearImGui/imgui_impl_dx11.h"
#include "../Engine/GraphicsEngine/Objects/TextureAsset.h"
#include "../Engine/GraphicsEngine/Objects/SpriteAsset.h"
#include "../Engine/GraphicsEngine/Commands/GCmdCustom.h"


#include "../Engine/Utilities/CommonUtilities/Input.h"
#include "../Engine/Utilities/CommonUtilities/Input.h"
#include "../Engine/Utilities/CommonUtilities/Timer.h"
#include <fstream>
#include <windows.h>
#include <commdlg.h>
#include <tchar.h>
#include <string>
#include <algorithm>
#include <MainSingleton.h>




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

	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
}


void GUI::Update()
{
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Editor");
	ImGui::Text("Hello");
	ImGui::End();
	//update editortools and other stuff
	//fetch info from scene about selected objects

	ImGui::Render();
	std::shared_ptr<GuiFrameData> frameToRender = std::make_shared<GuiFrameData>();
	CaptureDrawData(*frameToRender);

	MainSingleton::Get().GetRenderer().Enqueue<GCmdCustom>(RenderStage::UI, [this, frameToRender]()
		{
			// This lambda will run on the RENDER THREAD
			this->Render(*frameToRender);
		});
}

void GUI::CaptureDrawData(GuiFrameData& outFrame)
{
	ImDrawData* src = ImGui::GetDrawData();

	if (!src || src->CmdListsCount == 0 || !src->Valid)
		return;

	// NO MORE outFrame.Clear() HERE!

	outFrame.DrawData = *src;
	outFrame.ClonedLists.reserve(src->CmdListsCount);

	for (int i = 0; i < src->CmdListsCount; i++)
	{
		ImDrawList* clonedList = src->CmdLists[i]->CloneOutput();
		outFrame.ClonedLists.push_back(clonedList);
	}

	outFrame.DrawData.CmdLists.Data = nullptr;
	outFrame.DrawData.CmdLists.Size = 0;
	outFrame.DrawData.CmdLists.Capacity = 0;
}

void GUI::Render(const GuiFrameData& aFrame)
{
	if (aFrame.ClonedLists.empty()) return;

	ImGui_ImplDX11_NewFrame();

	// Fix up the pointers just for the duration of this call
	ImDrawData renderData = aFrame.DrawData;
	renderData.CmdLists.Data = (ImDrawList**)aFrame.ClonedLists.data();
	renderData.CmdLists.Size = (int)aFrame.ClonedLists.size();
	renderData.CmdLists.Capacity = (int)aFrame.ClonedLists.size();

	ImGui_ImplDX11_RenderDrawData(&renderData);
}


void GUI::ShutDown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}




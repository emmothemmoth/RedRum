#include "Editor.pch.h"
#include "GUI.h"
#include "GuiCmdInclude.h"

#include "../Engine/External/DearImGui/imgui_impl_win32.h"
#include "../Engine/External/DearImGui/imgui_impl_dx11.h"
#include "../Engine/GraphicsEngine/Objects/TextureAsset.h"
#include "../Engine/GraphicsEngine/Objects/SpriteAsset.h"
#include "../Engine/GraphicsEngine/Commands/GCmdCustom.h"
#include "../Engine/GraphicsEngine/Objects/TextureAsset.h"


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
#include "../Engine/GraphicsEngine/GraphicsEngine.h"




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


void GUI::Update(const float aDeltatime)
{
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport();
	//ImGui::SetNextWindowSize(ImVec2(1600, 800), ImGuiCond_FirstUseEver);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Scene Viewport");
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	unsigned int panelWidth = static_cast<unsigned int>(viewportPanelSize.x);
	unsigned int panelHeight = static_cast<unsigned int>(viewportPanelSize.y);
	// 1. Detect if the window size changed
	if (panelWidth != myPendingViewportSize.x || panelHeight != myPendingViewportSize.y)
	{
		myPendingViewportSize = { panelWidth, panelHeight };
		myResizeTimer = 0.15f; // Wait 150ms after they stop dragging
	}

	// 2. Tick down the timer (Assume you have DeltaTime available)
	if (myResizeTimer > 0.0f)
	{
		myResizeTimer -= aDeltatime; // Replace with your actual DeltaTime
		if (myResizeTimer <= 0.0f)
		{
			// 1. Tell the Update Thread systems (Camera, Logic) to update their math immediately
			OnViewportResize.Broadcast(myPendingViewportSize);
			myCurrentViewportSize = myPendingViewportSize;

			// 2. Capture the size safely by value for the lambda
			CU::Vector2<unsigned int> newSize = myPendingViewportSize;

			GraphicsEngine::Get().SetLogicalRenderSize({ static_cast<float>(newSize.x), static_cast<float>(newSize.y) });

			// 3. Enqueue the actual DirectX reallocation to the RENDER THREAD.
			// CRITICAL: Put this in a RenderStage that happens BEFORE your geometry pass!
			// (e.g., RenderStage::PreRender, RenderStage::Init, or your equivalent)
			MainSingleton::Get().GetRenderer().Enqueue<GCmdCustom>(RenderStage::PreRendering, [newSize]()
				{
					GraphicsEngine::Get().ResizeViewport(newSize.x, newSize.y);
				});
			ImGui::End();
			ImGui::PopStyleVar();
			ImGui::EndFrame();
			return;
		}
	}


	if (myViewportTexture)
	{
		// Display the texture! 
		// Cast the DX11 Shader Resource View directly to ImTextureID
		ImGui::Image((ImTextureID)myViewportTexture->GetSRV().Get(), viewportPanelSize);
	}
	
	ImGui::End();
	ImGui::PopStyleVar(); 

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




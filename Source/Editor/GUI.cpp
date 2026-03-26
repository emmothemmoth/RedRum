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
#include "../Engine/GraphicsEngine/GraphicsEngine.h"

#include "../Engine/AssetManager/AssetManager.h"

#include "MainSingleton.h"
#include "Components/AudioSourceComponent.h"
#include "Components/AudioListenerComponent.h"




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

	myContentRoot = AssetManager::Get().GetContentRootDirectory();
	myCurrentDirectory = myContentRoot;

	InitIcons();
}


void GUI::Update(const float aDeltatime)
{
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	DisplayViewport(aDeltatime);
	DisplayInspector();
	DisplayContentBrowser();

	ImGui::EndFrame();
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

void GUI::InitIcons()
{
	myFolderIcon = AssetManager::Get().GetAsset<TextureAsset>("T_FolderIcon_C");
	myMeshIcon = AssetManager::Get().GetAsset<TextureAsset>("T_MeshIcon_C");
	myAudioIcon = AssetManager::Get().GetAsset<TextureAsset>("T_AudioIcon_C");
	myFileIcon = AssetManager::Get().GetAsset<TextureAsset>("T_FileIcon_C");
}

void GUI::DisplayViewport(const float aDeltaTime)
{
	ImGui::Begin("Viewport", nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse);
	myViewportHovered = ImGui::IsWindowHovered();
	myViewportFocused = ImGui::IsWindowFocused();
	if (myViewportHovered && myViewportFocused)
	{
		ImVec2 mousePos = ImGui::GetMousePos();
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 cursorStart = ImGui::GetCursorScreenPos();
		ImVec2 contentPos = {
			mousePos.x - cursorStart.x,
			mousePos.y - cursorStart.y
		};
		CU::Vector2U cursor = { static_cast<unsigned>(contentPos.x), static_cast<unsigned>(contentPos.y) };
		myInterface.InterfaceUpdate(cursor, aDeltaTime);
	}
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	unsigned int panelWidth = static_cast<unsigned int>(viewportPanelSize.x);
	unsigned int panelHeight = static_cast<unsigned int>(viewportPanelSize.y);

	if (panelWidth != myPendingViewportSize.x || panelHeight != myPendingViewportSize.y)
	{
		myPendingViewportSize = { panelWidth, panelHeight };
		myResizeTimer = 0.15f;
	}

	if (myResizeTimer > 0.0f)
	{
		myResizeTimer -= aDeltaTime;
		if (myResizeTimer <= 0.0f)
		{
			OnViewportResize.Broadcast(myPendingViewportSize);
			myCurrentViewportSize = myPendingViewportSize;

			CU::Vector2<unsigned int> newSize = myPendingViewportSize;

			GraphicsEngine::Get().SetLogicalRenderSize({ static_cast<float>(newSize.x), static_cast<float>(newSize.y) });

			MainSingleton::Get().GetRenderer().Enqueue<GCmdCustom>(RenderStage::PreRendering, [newSize]()
				{
					GraphicsEngine::Get().ResizeViewport(newSize.x, newSize.y);
				});
			ImGui::End();
			ImGui::PopStyleVar();
			return;
		}
	}


	if (myViewportTexture)
	{
		ImGui::Image((ImTextureID)myViewportTexture->GetSRV().Get(), viewportPanelSize);
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

void GUI::DisplayInspector()
{
	ImGui::Begin("Inspector");
	auto& selection = myInterface.GetSelectedObjects();
	if (selection.size() > 1)
	{
		ImGui::Text("%d selected objects", static_cast<int>(selection.size()));
	}
	else if(selection.size() == 1)
	{
		const std::shared_ptr<Scene>& scene = myInterface.GetActiveScene();
		if (scene)
		{
			std::shared_ptr<GameObject> object = scene->GetObjectByID(selection.back());
			if (std::shared_ptr<MeshComponent> mesh = object->GetLastAddedComponent<MeshComponent>())
			{
				ImGui::Separator();
				ImGui::Text("Mesh: %s", mesh->GetMesh()->GetPath().string().c_str());
				ImGui::BulletText("Material Settings");
			}
			if (std::shared_ptr<AudioSourceComponent> audioSource = object->GetComponent<AudioSourceComponent>())
			{
				ImGui::Separator();
				ImGui::Text("Audio Source: %s", audioSource->GetAudioSourceName().string().c_str());
				ImGui::BulletText("Settings");
				ImGui::DragFloat("Volume", &audioSource->GetSettings().Volume, 0.5f, 0.0f, 6.0f);
				if (audioSource->IsPlayable())
				{
					if (ImGui::Button("Play"))
					{
						audioSource->Play();
					}
					if (ImGui::Button("Stop"))
					{
						audioSource->Stop();
					}
				}
			}
		}
	}
	ImGui::End();
}

void GUI::DisplayContentBrowser()
{
	ImGui::Begin("File browser");
	if (myCurrentDirectory != myContentRoot)
	{
		if (ImGui::Button("<- Back"))
		{
			myCurrentDirectory = myCurrentDirectory.parent_path();
		}
	}
	float padding = 16.0f;
	float thumbnailSize = 64.0f;
	float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = static_cast<int>(panelWidth / cellSize);
	if (columnCount < 1) columnCount = 1;


	if (ImGui::BeginTable("ContentBrowserTable", columnCount))
	{
		ImTextureID iconID;
		for (auto& directoryEntry : std::filesystem::directory_iterator(myCurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			std::string filenameString = path.filename().string();
			std::string extension = path.extension().string();

			ImGui::TableNextColumn();

			if (directoryEntry.is_directory())
			{
				iconID = (ImTextureID)myFolderIcon->GetSRV().Get();
			}
			else
			{
				if (extension.starts_with(".wav") || extension.starts_with(".aiff"))
				{
					iconID = (ImTextureID)myAudioIcon->GetSRV().Get();
				}
				else if (extension.starts_with(".fbx") || extension.starts_with(".FBX"))
				{
					iconID = (ImTextureID)myMeshIcon->GetSRV().Get();
				}
				else
				{
					iconID = (ImTextureID)myFileIcon->GetSRV().Get();
				}
			}
			ImGui::PushID(filenameString.c_str());
			if (ImGui::ImageButton(filenameString.c_str(), iconID, { thumbnailSize, thumbnailSize }))
			{
				if (directoryEntry.is_directory())
				{
					myCurrentDirectory /= path.filename();
				}
				else
				{
					// Maybe select the asset to show in a properties panel?
				}
			}
			ImGui::PopID();

			ImGui::TextWrapped("%s", filenameString.c_str());
		}

		ImGui::EndTable();
	}
	ImGui::Separator();
	ImGui::End();
}




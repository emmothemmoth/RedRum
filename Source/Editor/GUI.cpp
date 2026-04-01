#include "Editor.pch.h"
#include "GUI.h"
#include "GuiCmdInclude.h"

#include "../Engine/External/DearImGui/imgui_impl_win32.h"
#include "../Engine/External/DearImGui/imgui_impl_dx11.h"
#include "../Engine/GraphicsEngine/Objects/TextureAsset.h"
#include "../Engine/GraphicsEngine/Objects/SpriteAsset.h"
#include "../Engine/GraphicsEngine/Commands/GCmdCustom.h"


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
#include "Components/ListenerComponent.h"
#include "Components/BoxComponent.h"
#include "../Engine/External/DearImGui/imgui_internal.h"




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
	DisplayBuiltInTypes();
	DisplayToolbar();

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
	auto& assetManager = AssetManager::Get();
	myBuiltInTypes.at(static_cast<size_t>(BuiltInType::Wall)) = assetManager.GetAsset<TextureAsset>("T_WallIcon_C");
	myBuiltInTypes.at(static_cast<size_t>(BuiltInType::Mannequin)) = assetManager.GetAsset<TextureAsset>("T_MannequinIcon_C");
	myFolderIcon = assetManager.GetAsset<TextureAsset>("T_FolderIcon_C");
	myMeshIcon = assetManager.GetAsset<TextureAsset>("T_MeshIcon_C");
	myAudioIcon = assetManager.GetAsset<TextureAsset>("T_AudioIcon_C");
	myFileIcon = assetManager.GetAsset<TextureAsset>("T_FileIcon_C");
	myPlayIcon = assetManager.GetAsset<TextureAsset>("T_PlayIcon_C");
	myStopIcon = assetManager.GetAsset<TextureAsset>("T_StopIcon_C");
	myBakeIcon = assetManager.GetAsset<TextureAsset>("T_BakeIcon_C");
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
	if (myInterface.GetEditorMode() == EditorMode::Editing)
	{
		if (ImGui::BeginDragDropTarget())
		{
			// Check if the payload is from the Content Browser
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				// Extract the string from the payload memory
				const char* droppedPathStr = static_cast<const char*>(payload->Data);
				std::filesystem::path droppedPath(droppedPathStr);

				// Reuse the logic you already wrote for OS drag-and-drop!
				myInterface.OnExternalFileDropped(droppedPath);
			}

			// Check if the payload is from the Built-In Types window
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("BUILT_IN_ITEM"))
			{
				// Extract the enum from the payload memory
				BuiltInType droppedType = *static_cast<const BuiltInType*>(payload->Data);
				myInterface.OnInternalFileDropped(droppedType);
			}

			ImGui::EndDragDropTarget();
		}
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
			ImGui::BulletText("Transform");
			ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Padding before transforms

			// 1. Position
			CU::Vector3f position = object->GetPosition();
			if (DrawVec3Control("Translation", position, 0.0f))
			{
				object->SetPosition(position);
			}

			// 2. Rotation
			CU::Vector3f rotation = object->GetRotation();
			if (DrawVec3Control("Rotation", rotation, 0.0f))
			{
				object->SetRotation(rotation);
			}

			// 3. Scale (Pass 1.0f so clicking the button resets scale to 1!)
			CU::Vector3f scale = object->GetScale();
			if (DrawVec3Control("Scale", scale, 1.0f))
			{
				object->SetScale(scale);
			}

			ImGui::Separator();
			if (std::shared_ptr<ListenerComponent> listener = object->GetLastAddedComponent<ListenerComponent>())
			{
				ImGui::Separator();
				if (myInterface.GetEditorMode() == EditorMode::ListenerPOV)
				{
					if (ImGui::Button("Escape"))
					{
						object->GetComponent<MeshComponent>()->SetVisible(true);
						if (std::shared_ptr<MeshComponent> mesh = object->GetLastAddedComponent<MeshComponent>())
						{
							mesh->SetVisible(true);
						}
						if (std::shared_ptr<BoxComponent> box = object->GetLastAddedComponent<BoxComponent>())
						{
							box->SetVisible(true);
						}
						scene->ResetCamera();
						myInterface.SetEditorMode(EditorMode::Editing);
					}
				}
				else if(ImGui::Button("Enter POV"))
				{
					object->GetComponent<MeshComponent>()->SetVisible(false);
					if (std::shared_ptr<MeshComponent> mesh = object->GetLastAddedComponent<MeshComponent>())
					{
						mesh->SetVisible(false);
					}
					if (std::shared_ptr<BoxComponent> box = object->GetLastAddedComponent<BoxComponent>())
					{
						box->SetVisible(false);
					}
					scene->ChangeCamera(object);
					myInterface.SetEditorMode(EditorMode::ListenerPOV);
				}
			}
			else if (std::shared_ptr<MeshComponent> mesh = object->GetLastAddedComponent<MeshComponent>())
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

			if (!directoryEntry.is_directory())
			{
				// Tell ImGui the previous item (the ImageButton) is draggable
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					// 1. Copy the file path string into the payload memory
					std::string payloadPath = path.string();

					// "CONTENT_BROWSER_ITEM" is our unique ID. We pass the string and its size (+1 for null terminator)
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", payloadPath.c_str(), payloadPath.size() + 1);

					// 2. Display a tooltip next to the mouse while dragging!
					ImGui::Text("Drop to spawn: %s", filenameString.c_str());

					ImGui::EndDragDropSource();
				}
			}

			ImGui::TextWrapped("%s", filenameString.c_str());
		}

		ImGui::EndTable();
	}
	ImGui::Separator();
	ImGui::End();
}

void GUI::DisplayBuiltInTypes()
{
	ImGui::Begin("Built In Types");
	float padding = 16.0f;
	float thumbnailSize = 64.0f;
	float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = static_cast<int>(panelWidth / cellSize);
	if (columnCount < 1) columnCount = 1;


	if (ImGui::BeginTable("Types", columnCount))
	{
		ImTextureID iconID;
		for (int index = 0; index < static_cast<int>(BuiltInType::Count); ++index)
		{
			ImGui::TableNextColumn();
			iconID = (ImTextureID)myBuiltInTypes.at(index)->GetSRV().Get();
			std::string nameID = myBuiltInTypes.at(index)->GetName();
			ImGui::PushID(nameID.c_str());
			if (ImGui::ImageButton(nameID.c_str(), iconID, { thumbnailSize, thumbnailSize }))
			{

			}
			ImGui::PopID();
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				// 1. Copy the enum into the payload memory
				BuiltInType payloadType = static_cast<BuiltInType>(index);
				ImGui::SetDragDropPayload("BUILT_IN_ITEM", &payloadType, sizeof(BuiltInType));

				// 2. Display the tooltip
				ImGui::Text("Drop to spawn: %s", BuiltInTypeToString(payloadType).c_str());

				ImGui::EndDragDropSource();
			}
			ImGui::TextWrapped("%s", BuiltInTypeToString(static_cast<BuiltInType>(index)).c_str());
		
		}

		ImGui::EndTable();
	}
	ImGui::Separator();
	ImGui::End();
}

void GUI::DisplayToolbar()
{
	ImGui::Begin("Main Toolbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

	const std::shared_ptr<Scene>& scene = myInterface.GetActiveScene();
	if (scene)
	{
		std::shared_ptr<ListenerComponent> listener = scene->GetListener();

		if (listener)
		{
			ImVec2 iconSize(24.0f, 24.0f);

			// Get your Texture IDs (Casting your DirectX11 SRVs)
			ImTextureID bakeTex = (ImTextureID)myBakeIcon->GetSRV().Get();
			ImTextureID playTex = (ImTextureID)myPlayIcon->GetSRV().Get();
			ImTextureID stopTex = (ImTextureID)myStopIcon->GetSRV().Get();


			if (ImGui::ImageButton("BakeBtn", bakeTex, iconSize))
			{
				MainSingleton::Get().GetAudioEngine().StartRoomSimulation();
			}

			// Optional: Add a tooltip so users know what the icon does
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Bake Room Simulation");

			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::SameLine();

			// 2. Play & Stop Buttons
			bool isPlayable = listener->IsPlayable();
			ImGui::BeginDisabled(!isPlayable);

			if (ImGui::ImageButton("PlayBtn", playTex, iconSize))
			{
				listener->StartPreview();
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Play Simulation");

			ImGui::SameLine();

			if (ImGui::ImageButton("StopBtn", stopTex, iconSize))
			{
				listener->StopPreview();
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Stop Simulation");

			ImGui::EndDisabled();
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "No Listener in Scene!");
		}
	}

	ImGui::End();
}

std::string GUI::BuiltInTypeToString(BuiltInType aType)
{
	switch (aType)
	{
	case BuiltInType::Wall:
		return "Wall segment";
	case BuiltInType::Mannequin:
		return "Mannequin";
	case BuiltInType::Count:
		break;
	default:
		break;
	}
	return "";
}

bool GUI::DrawVec3Control(const std::string& aLabel, CU::Vector3f& someValues, float aResetValue)
{
	bool valueChanged = false;
	ImGui::PushID(aLabel.c_str());

	// 1. Text above the floats
	ImGui::Text("%s", aLabel.c_str());

	// Calculate widths to make it span the inspector beautifully
	float lineHeight = ImGui::GetFrameHeight();
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
	float widthEach = (ImGui::GetContentRegionAvail().x - buttonSize.x * 3.0f - ImGui::GetStyle().ItemSpacing.x * 2.0f) / 3.0f;

	// 2. X Axis (Red)
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	if (ImGui::Button("X", buttonSize)) { someValues.x = aResetValue; valueChanged = true; }
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::SetNextItemWidth(widthEach);
	if (ImGui::DragFloat("##X", &someValues.x, 0.1f, 0.0f, 0.0f, "%.2f")) valueChanged = true;
	ImGui::SameLine();

	// 3. Y Axis (Green)
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	if (ImGui::Button("Y", buttonSize)) { someValues.y = aResetValue; valueChanged = true; }
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::SetNextItemWidth(widthEach);
	if (ImGui::DragFloat("##Y", &someValues.y, 0.1f, 0.0f, 0.0f, "%.2f")) valueChanged = true;
	ImGui::SameLine();

	// 4. Z Axis (Blue)
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	if (ImGui::Button("Z", buttonSize)) { someValues.z = aResetValue; valueChanged = true; }
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::SetNextItemWidth(widthEach);
	if (ImGui::DragFloat("##Z", &someValues.z, 0.1f, 0.0f, 0.0f, "%.2f")) valueChanged = true;

	ImGui::PopID();

	// Add some visual spacing between the Transform elements
	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	return valueChanged;
}




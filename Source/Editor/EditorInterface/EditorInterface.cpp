#include "EditorInterface.h"
#include "../Tools/ToolsInclude.h"

#include "../AssetManager/AssetManager.h"

#include "Components/ComponentsInclude.h"
#include "Scene.h"

#include "../GraphicsEngine/Objects/MeshAsset.h"

#include "CommonUtilities/Input.h"
#include "CommonUtilities/RandomWrapper.h"

#include <cassert>

void EditorInterface::Init(std::shared_ptr<Scene> aScene)
{
	assert(aScene.get());
	myActiveScene = aScene;
	myAvailableTools.at(static_cast<size_t>(EditorToolType::ScreenPicker)) = std::make_shared<ScreenPicker>();
	myAvailableTools.at(static_cast<size_t>(EditorToolType::Move)) = std::make_shared<MoveTool>();
	for (auto& tool : myAvailableTools)
	{
		tool->Init(aScene);
	}
}

void EditorInterface::InterfaceUpdate(const CU::Vector2U& aCursorPos, const float aDeltaTime)
{
	UpdateInputState(aCursorPos);
	switch (myMode
)
	{
	case EditorMode::Editing:
		ToolUpdate(aDeltaTime);
		break;
	case EditorMode::ListenerPOV:
		break;
	default:
		break;
	}
}

const std::vector<uint32_t>& EditorInterface::GetSelectedObjects()const
{
	return myAvailableTools.at(static_cast<size_t>(myActiveTool))->GetSelectedObjects();
}

void EditorInterface::OnExternalFileDropped(const std::filesystem::path& aFilePath)
{
	AddFileObject(aFilePath);
}

void EditorInterface::OnInternalFileDropped(BuiltInType aType)
{
	assert(aType != BuiltInType::Count);
	AddBuiltInObject(aType);

}

void EditorInterface::AddFileObject(const std::filesystem::path& aFilePath)
{
	auto& assetManager = AssetManager::Get();
	if (assetManager.RegisterAndLoadAsset(aFilePath))
	{
		std::shared_ptr<GameObject> newObject;
		RegistryID objectType = assetManager.GetLastRegistered();
		switch (objectType.Type)
		{
		case AssetType::Audio:
		{
			std::string name = "AudioSource_" + aFilePath.filename().string();
			newObject = std::make_shared<GameObject>(name);
			newObject->AddComponent(std::make_shared<MeshComponent>(*newObject, assetManager.GetAsset<MeshAsset>("SM_Speaker")));
			newObject->AddComponent(std::make_shared<AudioSourceComponent>(*newObject));
			newObject->GetComponent<AudioSourceComponent>()->Init(aFilePath);
			newObject->SetIcon(ComponentType::AudioSource, {0.0f, 75.0f, 0.0f, 0.0f});
			newObject->SetPosition({ 0.0f, 0.0f, 0.0f });
			break;
		}
		case AssetType::Mesh:
		{
			std::string name = "Mesh_" + aFilePath.filename().string();
			newObject = std::make_shared<GameObject>(name);
			newObject->AddComponent(std::make_shared<MeshComponent>(*newObject, assetManager.GetAsset<MeshAsset>(objectType.Name)));
			newObject->SetIcon(ComponentType::Mesh);
			newObject->SetPosition({ 0.0f, 0.0f, 0.0f });
			break;
		}
		default:
			break;
		}
		myActiveScene->AddObject(newObject);
	}
}

void EditorInterface::AddBuiltInObject(BuiltInType aType)
{
	CU::RandomWrapper random;
	std::shared_ptr<GameObject> newObject;
	auto& assetManager = AssetManager::Get();

	switch (aType)
	{
	case BuiltInType::Wall:
	{
		newObject = std::make_shared<GameObject>("Wall");
		newObject->AddComponent(std::make_shared<MeshComponent>(*newObject, assetManager.GetAsset<MeshAsset>("WallMesh"), true, true));
		newObject->SetIcon(ComponentType::Mesh);
		break;
	}
	case BuiltInType::Mannequin:
	{
		int randomIndex = random.GetRandomInt(0, 1);
		newObject = std::make_shared<GameObject>(randomIndex == 0 ? "Mannequin_Male" : "Mannequin_Female");

		std::string meshName = (randomIndex == 0) ? "SM_MannequinMale" : "SM_MannequinFemale";
		newObject->AddComponent(std::make_shared<MeshComponent>(*newObject, assetManager.GetAsset<MeshAsset>(meshName), true, true));
		newObject->SetIcon(ComponentType::Mesh);
		break;
	}
	default:
		return;
	}

	newObject->SetPosition({ 0.0f, 0.0f, 0.0f });

	myActiveScene->AddObject(newObject);
}

void EditorInterface::UpdateInputState(const CU::Vector2U& aCursorPos)
{
	myInputState.MousePos = aCursorPos;
	myInputState.MouseDelta = { static_cast<int>(CU::Input::GetMousePositionDelta().x),
		static_cast<int>(CU::Input::GetMousePositionDelta().y)};
	myInputState.CTRL = CU::Input::GetKeyDown(CU::Keys::CONTROL) || CU::Input::GetKeyHeld(CU::Keys::CONTROL);
	myInputState.SHIFT = CU::Input::GetKeyDown(CU::Keys::SHIFT) || CU::Input::GetKeyHeld(CU::Keys::SHIFT);
	myInputState.MousePressed = CU::Input::GetKeyDown(CU::Keys::MOUSELBUTTON);
	myInputState.MouseHeld = CU::Input::GetKeyHeld(CU::Keys::MOUSELBUTTON);
	myInputState.MouseReleased = CU::Input::GetKeyUp(CU::Keys::MOUSELBUTTON);
}

void EditorInterface::ToolUpdate(const float aDeltaTime)
{
	myAvailableTools.at(static_cast<size_t>(myActiveTool))->Update(myInputState, aDeltaTime);
	if (myAvailableTools.at(static_cast<size_t>(myActiveTool))->Done())
	{
		SwitchTool();
		myAvailableTools.at(static_cast<size_t>(myActiveTool))->Update(myInputState, aDeltaTime);
	}
}

void EditorInterface::SwitchTool()
{
	switch (myActiveTool)
	{
	case EditorToolType::ScreenPicker:
		myActiveTool = EditorToolType::Move;
		break;
	case EditorToolType::Move:
		myActiveTool = EditorToolType::ScreenPicker;
		break;
	case EditorToolType::Count:
		break;
	default:
		break;
	}
}
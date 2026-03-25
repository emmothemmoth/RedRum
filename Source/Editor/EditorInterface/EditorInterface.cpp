#include "EditorInterface.h"
#include "../Tools/ToolsInclude.h"
#include "Scene.h"

#include "CommonUtilities/Input.h"

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
	ToolUpdate(aDeltaTime);
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

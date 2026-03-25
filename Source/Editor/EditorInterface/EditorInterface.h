#pragma once

#include "InputState.h"
#include "../Tools/IEditorTool.h"

#include <memory>
#include <array>

enum class EditorToolType
{
	ScreenPicker,
	Move,
	Count
};

class Scene;
class EditorInterface
{
public:
	EditorInterface() = default;
	~EditorInterface() = default;
	void Init(std::shared_ptr<Scene> aScene);

	void InterfaceUpdate(const CU::Vector2U& aCursorPos, const float aDeltaTime);
private:

	void UpdateInputState(const CU::Vector2U& aCursorPos);

	void ToolUpdate(const float aDeltaTime);

	void SwitchTool();

private:
	std::array<std::shared_ptr<IEditorTool>, static_cast<size_t>(EditorToolType::Count)> myAvailableTools;
	std::vector<uint32_t> mySelectedObjects;
	std::shared_ptr<Scene> myActiveScene = nullptr;
	EditorToolType myActiveTool = EditorToolType::ScreenPicker;
	InputState myInputState;
};

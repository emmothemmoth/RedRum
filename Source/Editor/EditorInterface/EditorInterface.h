#pragma once

#include "InputState.h"
#include "../Tools/IEditorTool.h"
#include "BuiltInTypes.h"

#include <memory>
#include <array>

enum class EditorMode
{
	Editing,
	ListenerPOV
};

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

	void SetEditorMode(EditorMode aMode) { myMode = aMode; }
	EditorMode GetEditorMode() const { return myMode; }

	void InterfaceUpdate(const CU::Vector2U& aCursorPos, const float aDeltaTime);

	const std::vector<uint32_t>& GetSelectedObjects() const;
	std::shared_ptr<Scene> GetActiveScene() const { return myActiveScene; }

	void OnExternalFileDropped(const std::filesystem::path& aFilePath);

	void OnInternalFileDropped(BuiltInType aType);
private:
	void AddFileObject(const std::filesystem::path& aFilePath);
	void AddBuiltInObject(BuiltInType aType);

	void UpdateInputState(const CU::Vector2U& aCursorPos);

	void ToolUpdate(const float aDeltaTime);

	void SwitchTool();

private:
	std::array<std::shared_ptr<IEditorTool>, static_cast<size_t>(EditorToolType::Count)> myAvailableTools;
	std::vector<uint32_t> mySelectedObjects;
	std::shared_ptr<Scene> myActiveScene = nullptr;
	EditorMode myMode = EditorMode::Editing;
	EditorToolType myActiveTool = EditorToolType::ScreenPicker;
	InputState myInputState;
};

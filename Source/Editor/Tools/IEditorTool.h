#pragma once
#include "../EditorInterface/InputState.h"

#include "../GameEngine/Scene.h"

#include <memory>
#include <vector>

class IEditorTool
{
public:
	void Init(std::shared_ptr<Scene> aScene) { myScene = aScene; };

	virtual void Update(const InputState& anInputState, const float aDeltaTime) = 0;
	virtual bool Done() = 0;

	const std::vector<uint32_t>& GetSelectedObjects() const { return mySelectedObjects; }
protected:
	virtual void PerformAction(const InputState& anInputState) = 0;
protected:
	std::shared_ptr<Scene> myScene;
	static 	std::vector<uint32_t> mySelectedObjects;
	static uint32_t mySelectedPartID;
};

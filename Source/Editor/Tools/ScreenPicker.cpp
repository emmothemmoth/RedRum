#include "ScreenPicker.h"

#include "MainSingleton.h"
#include "../../Engine/GraphicsEngine/Commands/GCmdScreenPicking.h"
#include "../../Engine/GraphicsEngine/GraphicsEngine.h"

void ScreenPicker::Update(const InputState& anInputState, const float aDeltaTime)
{
	aDeltaTime;
	if (anInputState.MousePressed)
	{
		PerformAction(anInputState);
	}
}

bool ScreenPicker::Done()
{
	bool resultDone = false;
	unsigned ID = 0;
	GraphicsEngine::Get().ScreenPickingResult(resultDone, ID);
	if (resultDone && ID > 0)
	{
		uint32_t objectID = ID >> 8;
		uint32_t partID = ID & 0xFF;
		mySelectedPartID = partID;
		if (partID != 0)
		{
			return true;
		}
		bool isAlreadySelected = false;
		for (auto& id : mySelectedObjects)
		{
			if (id == objectID)
			{
				isAlreadySelected = true;
			}
			if (!myShiftHeld)
			{
				myScene->GetObjectByID(id)->OnDeselected();
			}
		}
		if (!myShiftHeld)
		{
			mySelectedObjects.clear();
		}
		if (!isAlreadySelected)
		{
			myScene->GetObjectByID(objectID)->OnSelected();
			mySelectedObjects.push_back(objectID);
		}
	}
	return false;
}


void ScreenPicker::PerformAction(const InputState& anInputState)
{
	myShiftHeld = anInputState.SHIFT;
	MainSingleton::Get().GetRenderer().Enqueue<GCmdScreenPicking>(RenderStage::PostProcess, anInputState.MousePos);
}


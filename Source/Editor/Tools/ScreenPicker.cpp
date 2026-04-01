#include "ScreenPicker.h"

#include "MainSingleton.h"
#include "../../Engine/GraphicsEngine/Commands/GCmdScreenPicking.h"
#include "../../Engine/GraphicsEngine/GraphicsEngine.h"

#include <chrono>

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
	unsigned ID = 0;
	if (myPickingFuture.valid() && myPickingFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		ID = myPickingFuture.get();
		uint32_t objectID = ID >> 8;
		uint32_t partID = ID & 0xFF;
		mySelectedPartID = partID;
		if (objectID == 0)
		{
			if (!myShiftHeld)
			{
				for (auto& id : mySelectedObjects)
				{
					myScene->GetObjectByID(id)->OnDeselected();
				}
				mySelectedObjects.clear();
			}
			return false;
		}
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

	auto promise = std::make_shared<std::promise<uint32_t>>();
	myPickingFuture = promise->get_future();
	MainSingleton::Get().GetRenderer().Enqueue<GCmdScreenPicking>(RenderStage::PostProcess, 
		anInputState.MousePos.x, 
		anInputState.MousePos.y, 
		promise);
}


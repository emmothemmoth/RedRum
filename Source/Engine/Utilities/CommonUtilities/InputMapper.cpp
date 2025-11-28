#include "CommonUtilities.pch.h"
#include "InputMapper.h"


static CommonUtilities::InputMapper localInputMapper;


CommonUtilities::InputMapper::InputMapper()
{
}

void CommonUtilities::InputMapper::Refresh()
{
	CommonUtilities::Input::Update();


	if (CommonUtilities::Input::GetKeyHeld(Keys::MOUSERBUTTON))
	{
		TranslateToEvent(GameInput::Rotation, 1.0f);

		if (CommonUtilities::Input::GetKeyHeld(Keys::A))
		{
			TranslateToEvent(GameInput::Move_Left, 1.0f);
		}
		if (CommonUtilities::Input::GetKeyHeld(Keys::W))
		{
			TranslateToEvent(GameInput::Move_Forward, 1.0f);
		}
		if (CommonUtilities::Input::GetKeyHeld(Keys::S))
		{
			TranslateToEvent(GameInput::Move_Backwards, 1.0f);
		}
		if (CommonUtilities::Input::GetKeyHeld(Keys::D))
		{
			TranslateToEvent(GameInput::Move_Right, 1.0f);
		}
		if (CommonUtilities::Input::GetKeyHeld(Keys::E) || CommonUtilities::Input::GetKeyHeld(Keys::SPACE))
		{
			TranslateToEvent(GameInput::Fly_Up, 1.0f);
		}
		if (CommonUtilities::Input::GetKeyHeld(Keys::Q) || (CommonUtilities::Input::GetKeyHeld(Keys::SPACE) && CommonUtilities::Input::GetKeyHeld(Keys::CONTROL)))
		{
			TranslateToEvent(GameInput::Fly_Down, 1.0f);
		}
	}
	if (CommonUtilities::Input::GetKeyDown(Keys::F1))
	{
		TranslateToEvent(GameInput::ToggleDirectionalLight, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyDown(Keys::F2))
	{
		TranslateToEvent(GameInput::TogglePointLight, 1.0f);;
	}
	if (CommonUtilities::Input::GetKeyDown(Keys::F3))
	{
		TranslateToEvent(GameInput::ToggleSpotLight, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyDown(Keys::F6))
	{
		TranslateToEvent(GameInput::ChangeTonemap, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyDown(Keys::OEM_PLUS))
	{
		TranslateToEvent(GameInput::ExposureUp, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyDown(Keys::OEM_MINUS))
	{
		TranslateToEvent(GameInput::ExposureDown, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyDown(Keys::F8))
	{
		TranslateToEvent(GameInput::ChangeLuminanceMode, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyDown(Keys::F7))
	{
		TranslateToEvent(GameInput::Toggle_SSAO, 1.0f);
	}
}

CommonUtilities::InputMapper& CommonUtilities::InputMapper::GetInputMapper()
{
	return localInputMapper;
}


void CommonUtilities::InputMapper::PostEvent(const ActionEvent& anEvent)
{
	if (myObservers->size() == 0)
	{
		return;
	}
	for (auto& observer : myObservers[static_cast<int>(anEvent.Id)])
	{
		if (observer != nullptr)
		{
			observer->RecieveEvent(anEvent);
		}
	}
}

void CommonUtilities::InputMapper::Register(const ActionEventID& anEventType, InputObserver* anObserver)
{
	myObservers[static_cast<int>(anEventType)].push_back(anObserver);
}

void CommonUtilities::InputMapper::UnRegister(const ActionEventID& anEventType, InputObserver* anObserver)
{
	auto list = myObservers[static_cast<int>(anEventType)];
	for (size_t index = 0; index < list.size(); ++index)
	{
		if (list[index] == anObserver)
		{
			list[index] = list[list.size() - 1];
			list.pop_back();
			return;
		}
	}
}

void CommonUtilities::InputMapper::BindEvent(const GameInput& anInput, const ActionEventID& anAction)
{
	myMappedInputs.insert(std::make_pair(anInput, anAction));
}

void CommonUtilities::InputMapper::TranslateToEvent(const GameInput& anInput, const float& aWeight)
{
	if (myMappedInputs.find(anInput) != myMappedInputs.end())
	{
		ActionEvent event = {};
		event.Id = myMappedInputs.at(anInput);
		event.Weight = aWeight;
		PostEvent(event);
	}
	return;
}

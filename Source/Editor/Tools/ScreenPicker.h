#pragma once
#include "IEditorTool.h"


class ScreenPicker : public IEditorTool
{
public:
	virtual void Update(const InputState& anInputState, const float aDeltaTime) override;
	virtual bool Done() override;

	virtual void PerformAction(const InputState& anInputState) override;
private:
private:
	bool myShiftHeld = false;
};

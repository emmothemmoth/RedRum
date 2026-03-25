#pragma once
#include "IEditorTool.h"

#include "CommonUtilities/Plane.hpp"

struct GizmoDragState
{
	bool Active = false;
	CU::Vector3f InitialIntersection;
	CU::Vector3f InitialObjectPosition;
	CU::Plane<float> DragPlane;
};

class MoveTool : public IEditorTool
{
public:

	virtual void Update(const InputState& anInputState, const float aDeltaTime) override;
	virtual bool Done() override;
private:
	virtual void PerformAction(const InputState& anInputState) override;

private:
	GizmoDragState myDragState;
	float currentDeltaTime = 0.0f;
};

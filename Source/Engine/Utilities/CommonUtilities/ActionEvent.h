#pragma once

enum class ActionEventID
{
	CameraMove_Left,
	CameraMove_Right,
	CameraMove_Forward,
	CameraMove_Backwards,
	CameraMove_Up,
	CameraMove_Down,
	CameraRotation,
	Toggle_DirectionalLight,
	Toggle_PointLights,
	Toggle_SpotLights,
	ChangeTonemap,
	ExposureUp,
	ExposureDown,
	ChangeLuminanceMode,
	Toggle_SSAO,
	Count
};

struct ActionEvent
{
	ActionEventID Id;
	float Weight;
};

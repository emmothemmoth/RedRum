#pragma once

enum class RenderStage
{
	PreRendering = 0,
	ShadowMapping = 1,
	Deferred = 2,
	Forward = 3,
	Particles = 4,
	Custom = 5,
	ObjectIDRendering = 6,
	PostProcess = 7,
	WorldSpaceUI = 8,
	Sprite = 9,
	UI = 10,
	Count = 11
};

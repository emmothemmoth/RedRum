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
	ObjectPartIDRendering = 7,
	PostProcess = 8,
	WorldSpaceUI = 9,
	Sprite = 10,
	UI = 11,
	Count = 11
};

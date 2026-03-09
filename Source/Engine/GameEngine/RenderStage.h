#pragma once

enum class RenderStage
{
	ShadowMapping = 0,
	Deferred = 1,
	Forward = 2,
	Particles = 3,
	Custom = 4,
	ObjectIDRendering = 5,
	PostProcess = 6,
	WorldSpaceUI = 7,
	Sprite = 8,
	Count = 9
};

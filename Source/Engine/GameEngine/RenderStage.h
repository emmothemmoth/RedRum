#pragma once

enum class RenderStage
{
	ShadowMapping = 0,
	Deferred = 1,
	Forward = 2,
	Particles = 3,
	Debug = 4,
	Custom = 5,
	WorldSpaceUI = 6,
	ObjectIDRendering = 7,
	PostProcess = 8,
	Sprite = 9,
	Count = 9
};

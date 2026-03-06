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
	PostProcess = 7,
	Sprite = 8,
	Count = 9
};

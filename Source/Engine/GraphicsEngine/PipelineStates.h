#pragma once

enum class PipelineStates
{
	DirlightShadowMapping,
	PointlightShadowMapping,
	SpotlightShadowMapping,
	DeferredRendering,
	DirlightRendering,
	PointlightRendering,
	SpotlightRendering,
	ForwardRendering,
	CustomRendering,
	ParticleRendering,
	CustomPostProcess,
	Tonemap,
	Resampling,
	GaussianV,
	GaussianH,
	Luminance,
	Bloom,
	SSAO,
	SpriteRendering,
};

#pragma once

enum class RenderTargets
{
	Backbuffer,
	ShadowMap
};

enum class Samplers
{
	DefaultSampler,
	BRDFSampler,
	ShadowSampler
};

enum class TextureResources
{
	ShadowTexture,

};

enum class DepthStencils
{
	Shadow,
	DepthBuffer
};
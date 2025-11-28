#pragma once
#include <wrl.h>

struct ID3D11BlendState;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11InputLayout;

class TextureAsset;
struct Shader;

enum class PSOType
{
	Forward,
	Shadow,
	PointShadow,
	SpotShadow,
	Deferred,
	DirLight,
	PointLight,
	SpotLight,
	Intermediate,
	Sprite,
	Particle,
	Tonemap,
	Luminance,
	Resampling,
	GaussianH,
	GaussianV,
	Bloom,
	SSAO,
	Custom
};

struct PipelineStateObject
{
	PipelineStateObject();
	PipelineStateObject(const PipelineStateObject& anObject) = default;
	~PipelineStateObject();

	Microsoft::WRL::ComPtr<ID3D11BlendState> BlendState;
	Microsoft::WRL::ComPtr <ID3D11RasterizerState> RasterizerState;
	Microsoft::WRL::ComPtr <ID3D11DepthStencilState> DepthStencilState;
	Microsoft::WRL::ComPtr <ID3D11InputLayout> InputLayout;


	std::array <std::shared_ptr<TextureAsset>, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> RenderTarget;
	std::array <bool, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> ClearRenderTarget;
	std::shared_ptr<TextureAsset> DepthStencil;
	bool ClearDepthStencil = false;

	std::unordered_map<unsigned, Microsoft::WRL::ComPtr<ID3D11SamplerState>> SamplerStates;

	PSOType Type;
	int RenderTargetCount;
};


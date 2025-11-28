#pragma once
#include <wrl.h>
#include <filesystem>

struct ID3D11DeviceChild;

enum class ShaderType : unsigned
{
	Unknown,
	VertexShader,
	GeometryShader,
	PixelShader
};

struct Shader
{
public:
	virtual ~Shader() = default;
	ShaderType type = ShaderType::Unknown;
	Microsoft::WRL::ComPtr<ID3D11DeviceChild> shader;
	std::filesystem::path path;
};

struct VertexShader : public Shader
{
public:
	~VertexShader() override = default;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	unsigned VertexStride = 0;
};


struct GeometryShader : public Shader
{
public:
	~GeometryShader() override = default;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	unsigned VertexStride = 0;
};

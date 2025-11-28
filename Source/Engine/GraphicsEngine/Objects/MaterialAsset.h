#pragma once
#include <memory>
#include <string>

#include "CommonUtilities\Vector.hpp"

#include "..\Buffers\ConstantBuffer.h"
#include "..\Buffers\MaterialBuffer.h"
#include "..\Shader.h"
#include "IAsset.h"

#include <filesystem>


struct PipelineStateObject;
class TextureAsset;


class MaterialAsset : public IAsset
{
	struct MaterialTextureBinding
	{
		unsigned Slot{};
		unsigned Stage{};
		std::wstring Name;
		std::shared_ptr<TextureAsset> Texture;
	};
public:
	unsigned myPipeLineStages = 0;

	std::wstring myPath;
	std::wstring myName;


public:
	MaterialAsset() = default;

	//MaterialAsset(const std::string_view& aName, const std::weak_ptr<Shader>& aVertexShader, const std::weak_ptr<Shader>& aPixelShader);
	//MaterialAsset(const std::filesystem::path& aPath);
	//MaterialAsset(const std::filesystem::path& aPath, const std::weak_ptr<Shader>& aVertexShader, const std::weak_ptr<Shader>& aPixelShader);
	MaterialAsset(const std::filesystem::path& aPath);

	MaterialBufferData& GetMaterialBuffer();

	//Issue RHI commands to set shaders and cbuffer on GPU
	//void SetAsResource(bool aSetPixelShader);

	void SetTexture(const std::wstring& aName, const std::weak_ptr<TextureAsset>& aTexture);

	std::shared_ptr<TextureAsset> GetAlbedoTexture();
	std::shared_ptr<TextureAsset> GetNormalTexture();
	std::shared_ptr<TextureAsset> GetMaterialTexture();
	std::shared_ptr<TextureAsset> GetFXTexture();

	CU::Vector4f& GetAlbedoTint();
	void SetAlbedoTint(CU::Vector4f aAlbedoTint);

	void SetMaterial(const MaterialBufferData& someData);

	//void SetShaders(const std::weak_ptr<Shader>& aVertexShader, const std::weak_ptr<Shader>& aPixelShader);

	bool Load() override;

private:

	MaterialBufferData myMaterialBuffer;

	std::vector<MaterialTextureBinding> myTextures;
};

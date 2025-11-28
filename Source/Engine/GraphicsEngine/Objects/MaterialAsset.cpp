#include "GraphicsEngine.pch.h"
#include "MaterialAsset.h"

#include "../../AssetManager/MaterialAssetHandler.h"
#include "../AssetManager/AssetManager.h"
#include "GraphicsEngine.h"

#include "TextureAsset.h"


//MaterialAsset::MaterialAsset(const std::string_view& aName, const std::weak_ptr<Shader>& aVertexShader, const std::weak_ptr<Shader>& aPixelShader)
//{
//	myVertexShader = aVertexShader;
//	myPixelShader = aPixelShader;
//	myMaterialBuffer.Initialize(aName); //to string view from my name
//	myPath = L"";
//	myMaterialBuffer.Data.AlbedoColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
//	myMaterialBuffer.Data.UVTiling = Vector2f(1.0f, 1.0f);
//	myMaterialBuffer.Data.NormalStrength = 1.0f;
//	myMaterialBuffer.Data.Shininess = 1.0f;
//}
//
//MaterialAsset::MaterialAsset(const std::filesystem::path& aPath)
//{
//	myName = aPath.stem().wstring();
//	myPath = aPath;
//	myMaterialBuffer.Initialize(aPath.stem().string());
//	myMaterialBuffer.Data.AlbedoColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
//	myMaterialBuffer.Data.UVTiling = Vector2f(1.0f, 1.0f);
//	myMaterialBuffer.Data.NormalStrength = 1.0f;
//	myMaterialBuffer.Data.Shininess = 1.0f;
//
//}

MaterialAsset::MaterialAsset(const std::filesystem::path& aPath)
{
	myName = aPath.stem().wstring();

	myMaterialBuffer.AlbedoColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	myMaterialBuffer.UVTiling = Vector2f(1.0f, 1.0f);
	myMaterialBuffer.NormalStrength = 1.0f;
	myMaterialBuffer.Shininess = 1.0f;
	myMaterialBuffer.EmissionStrength = 0.0f;
	myPath = aPath;
}

MaterialBufferData& MaterialAsset::GetMaterialBuffer()
{
	return myMaterialBuffer;
}

//void MaterialAsset::SetAsResource(bool aSetPixelShader)
//{
//	aSetPixelShader;
//	GraphicsEngine::Get().GetRHI()->UpdateConstantBufferData(myMaterialBuffer);
//	GraphicsEngine::Get().GetRHI()->SetConstantBuffer(PIPELINE_STAGE_PIXEL_SHADER, 3, myMaterialBuffer);
//
//	//Set all textures as resources
//	for (const auto& binding : myTextures)
//	{
//		std::shared_ptr<TextureAsset> texture(binding.Texture);
//		GraphicsEngine::Get().GetRHI()->SetTextureResource(binding.Stage, binding.Slot, *texture);
//	}
//	
//	//Detta görs av PSOn numer
//	//Set shaders
//	//std::shared_ptr<Shader> vShader(myPSO->VertexShader);
//	//GraphicsEngine::Get().GetRHI()->SetVertexShader(vShader.get());
//	//if (aSetPixelShader)
//	//{
//	//	std::shared_ptr<Shader> pShader(myPSO->PixelShader);
//	//	GraphicsEngine::Get().GetRHI()->SetPixelShader(pShader.get());
//	//}
//	//else
//	//{
//	//	GraphicsEngine::Get().GetRHI()->PSSetShader(nullptr, nullptr, 0);
//	//}
//}	//

void MaterialAsset::SetTexture(const std::wstring& aName, const std::weak_ptr<TextureAsset>& aTexture)
{
	MaterialTextureBinding newTexture = {};
	newTexture.Name = aName;
	newTexture.Texture = aTexture.lock();
	newTexture.Slot = static_cast<unsigned>(myTextures.size());
	newTexture.Stage = static_cast<unsigned>(PIPELINE_STAGE_PIXEL_SHADER);
	myTextures.push_back(newTexture);
}

std::shared_ptr<TextureAsset> MaterialAsset::GetAlbedoTexture()
{
	if (myTextures.size() >= 1)
	{
		return myTextures[0].Texture;
	}
	return std::shared_ptr<TextureAsset>();
}

std::shared_ptr<TextureAsset> MaterialAsset::GetNormalTexture()
{
	if (myTextures.size() >= 2)
	{
		return myTextures[1].Texture;
	}
	return std::shared_ptr<TextureAsset>();
}

std::shared_ptr<TextureAsset> MaterialAsset::GetMaterialTexture()
{
	if (myTextures.size() >= 3)
	{
		return myTextures[2].Texture;
	}
	return std::shared_ptr<TextureAsset>();
}

std::shared_ptr<TextureAsset> MaterialAsset::GetFXTexture()
{
	if (myTextures.size() >= 4)
	{
		return myTextures[3].Texture;
	}
	return std::shared_ptr<TextureAsset>();
}

CU::Vector4f& MaterialAsset::GetAlbedoTint()
{
	return myMaterialBuffer.AlbedoColor;
}

void MaterialAsset::SetAlbedoTint(CU::Vector4f aAlbedoTint)
{
	myMaterialBuffer.AlbedoColor = aAlbedoTint;
}

void MaterialAsset::SetMaterial(const MaterialBufferData& someData)
{
	myMaterialBuffer.AlbedoColor = someData.AlbedoColor;
	myMaterialBuffer.NormalStrength = someData.NormalStrength;
	myMaterialBuffer.Shininess = someData.Shininess;
	myMaterialBuffer.EmissionStrength = someData.EmissionStrength;
	myMaterialBuffer.UVTiling = someData.UVTiling;
}

//void MaterialAsset::SetShaders(const std::weak_ptr<Shader>& aVertexShader, const std::weak_ptr<Shader>& aPixelShader)
//{
//	mymyPixelShader = aPixelShader;
//	myVertexShader = aVertexShader;
//}

bool MaterialAsset::Load()
{
 	bool wasLoaded = true;

	wasLoaded = MaterialAssetHandler::Get().LoadMaterialFromJSon(myPath,*this);
	for (auto& texture : myTextures)
	{
		std::shared_ptr<TextureAsset> text = AssetManager::Get().GetAsset<TextureAsset>(texture.Name);
		texture.Texture = text;
	}

	return wasLoaded;
}

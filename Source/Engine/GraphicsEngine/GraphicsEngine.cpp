#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"

#include "WindowHandler.h"
#include "PipelineStates.h"

#include "..\Shaders\Include\Default_C.h"
#include "..\Shaders\Include\Default_N.h"
#include "..\Shaders\Include\Default_M.h"
#include "..\Shaders\Include\Default_FX.h"


#include "Objects/Vertex.h"
#include "Objects/ParticleVertex.h"
#include "Objects/ParticleEmitter.h"
#include "Objects/TextureAsset.h"
#include "Objects/MaterialAsset.h"
#include "Objects\SpriteAsset.h"
#include "Objects/DebugLineObject.h"
#include "Buffers/LightBuffer.h"
#include "Buffers\FrameBuffer.h"
#include "Buffers\ObjectBuffer.h"
#include "Buffers\LightBuffer.h"
#include "Buffers\GBuffer.h"
#include "Buffers\MaterialBuffer.h"
#include "Buffers\DebugBuffer.h"
#include "Buffers\PostProcessBuffer.h"
#include "GraphicsCommands.h"
#include "CommonUtilities\Matrix.hpp"
#include "CommonUtilities\UtilityFunctions.hpp"

#include "Logger\Logger.h"
#include <random>

#include <filesystem>
#include <utility>
#include <Objects\InstanceData.h>



#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(GELog, "GE", Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(GELog, "GE", Error);
#endif

DEFINE_LOG_CATEGORY(GELog);

GraphicsEngine& GraphicsEngine::Get()
{
	static GraphicsEngine myInstance;
	return myInstance;
}

const std::unique_ptr<RHI>& GraphicsEngine::GetRHI()
{
	return Get().myRHI;
}

CU::Vector2<unsigned> GraphicsEngine::GetDeviceSize() const
{
	RECT clientRect{};
	GetClientRect(myWindowHandler->GetWindowHandle(), &clientRect);

	const unsigned clientWidth = clientRect.right - clientRect.left;
	const unsigned clientHeight = clientRect.bottom - clientRect.top;

	return { clientWidth, clientHeight };
}

CU::Vector2f GraphicsEngine::GetViewPortSize()
{
	return myRHI->GetViewPortSize();
}

ViewPort GraphicsEngine::GetViewPort()
{
	return myRHI->GetViewPort();
}

bool GraphicsEngine::InitWindow(SIZE aWindowSize, WNDPROC aWindowProcess, LPCWSTR aWindowTitle)
{
	myWindowHandler = std::make_unique<WindowHandler>();
	myWindowHandler->Init(aWindowSize, aWindowProcess, aWindowTitle);
	return false;
}

void GraphicsEngine::ShowSplashScreen()
{
	myWindowHandler->ShowSplashWindow();
}

void GraphicsEngine::HideSplashScreen()
{
	myWindowHandler->HideSplashWindow();
}

bool GraphicsEngine::Initialize(bool enableDeviceDebug)
{
	enableDeviceDebug;
		myRHI = std::make_unique<RHI>();
#ifdef _DEBUG
		if (!myRHI->Initialize(myWindowHandler->GetWindowHandle(),
			enableDeviceDebug))
		{
			LOG(GELog, Error, "Failed to initialize RHI!");
			return false;
		}
#else
		if (!myRHI->Initialize(myWindowHandler->GetWindowHandle(),
			false))
		{
			LOG(GELog, Error, "Failed to initialize RHI!");
			return false;
		}

#endif

		CreateShaders();
		CreateUIShaders();
		CreateAudioShaders();
		CreateParticleShaders();
		CreatePostProcessShaders();
		myShaderMap;
		CreateIntermediateBuffers();

		if (!CreateForwardPSO())
		{
			LOG(GELog, Error, "Failed to create default PSO");
			return false;
		}
		if (!CreateLUTTexture())
		{
			LOG(GELog, Error, "Failed to create LUT Texture");
			return false;
		}

		//Environment cube map loading (for BRDF)
		std::filesystem::path relativeTexturePath = "Content\\CubeMaps\\skansen_cubemap.dds";
		TextureAsset envCubeTexture;
		LoadTexture(relativeTexturePath, envCubeTexture);
		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 120, envCubeTexture))
		{
			LOG(GELog, Error, "Failed to initialize Skansen cube");
		}

		std::filesystem::path relativeNoisePath = "Content\\Textures\\T_BlueNoise.dds";
		myBluenoiseTexture = std::make_shared<TextureAsset>();
		myBluenoiseTexture->SetName("Bluenoise texture");
		if (!LoadTexture(relativeNoisePath, *myBluenoiseTexture))
		{
			LOG(GELog, Error, "Failed to load Blue noise texture");
		}
		//if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 4, *myBluenoiseTexture))
		//{
		//	LOG(GELog, Error, "Failed to set Blue noise texture on slot 4");
		//}


		if (!CreateDirLightShadowMap("DirlightShadowMap"))
		{
			LOG(GELog, Error, "Failed to create Shadow map");
			return false;
		}
		CreatePointLightShadowMaps("PointlightShadowMaps");
		CreateSpotLightShadowMaps("SpotlightShadowMaps");

		if (!CreateGBuffer())
		{
			LOG(GELog, Error, "Failed to create Graphics buffer");
			return false;
		}
		if (!CreateShadowPSO())
		{
			LOG(GELog, Error, "Failed to create Shadow PSO");
			return false;
		}
		if (!CreatePointShadowPSO())
		{
			LOG(GELog, Error, "Failed to create PointShadow PSO");
			return false;
		}
		if (!CreateSpotShadowPSO())
		{
			LOG(GELog, Error, "Failed to create SpotShadow PSO");
			return false;
		}
		if (!CreateDirLightPSO())
		{
			LOG(GELog, Error, "Failed to create Dirlight PSO");
			return false;
		}
		if (!CreatePointLightPSO())
		{
			LOG(GELog, Error, "Failed to create PointLight PSO");
			return false;
		}
		if (!CreateSpotLightPSO())
		{
			LOG(GELog, Error, "Failed to create SpotLight PSO");
			return false;
		}
		if (!CreateDeferredPSO())
		{
			LOG(GELog, Error, "Failed to create Deferred PSO");
			return false;
		}
		if (!CreateSpritePSO())
		{
			LOG(GELog, Error, "Failed to create sprite PSO");
			return false;
		}
		if (!CreateTonemapPSO())
		{
			LOG(GELog, Error, "Failed to create tonemap PSO");
			return false;
		}
		if (!CreateResamplingPSO())
		{
			LOG(GELog, Error, "Failed to create resampling PSO");
			return false;
		}
		if (!CreateLuminancePSO())
		{
			LOG(GELog, Error, "Failed to create luminance PSO");
			return false;
		}
		if (!CreateGaussianHPSO())
		{
			LOG(GELog, Error, "Failed to create gaussianH PSO");
			return false;
		}
		if (!CreateGaussianVPSO())
		{
			LOG(GELog, Error, "Failed to create gaussianV PSO");
			return false;
		}
		if (!CreateBloomPSO())
		{
			LOG(GELog, Error, "Failed to create bloom PSO");
			return false;
		}
		if (!CreateSSAOPSO())
		{
			LOG(GELog, Error, "Failed to create SSAO PSO");
			return false;
		}
		if (!CreateParticlePSO())
		{
			LOG(GELog, Error, "Failed to create Particle PSO");
			return false;
		}

		if (!CreateDefaultMaterials())
		{
			LOG(GELog, Error, "Failed to create default materials");
			return false;
		}


		ConstantBuffer frameBuffer;
		if (!myRHI->CreateConstantBuffer("FrameBuffer", sizeof(FrameBufferData), 0, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER, frameBuffer))
		{
			LOG(GELog, Error, "Failed to create frame buffer");
			return false;
		}

		ConstantBuffer objectBuffer;
		if (!myRHI->CreateConstantBuffer("ObjectBuffer", sizeof(ObjectBufferData), 1, PIPELINE_STAGE_VERTEX_SHADER, objectBuffer))
		{
			LOG(GELog, Error, "Failed to create object buffer");
			return false;
		}

		ConstantBuffer materialBuffer;
		if (!myRHI->CreateConstantBuffer("MaterialBuffer", sizeof(MaterialBufferData), 3, PIPELINE_STAGE_PIXEL_SHADER, materialBuffer))
		{
			LOG(GELog, Error, "Failed to create material buffer");
			return false;
		}

		ConstantBuffer lightBuffer;
		if (!myRHI->CreateConstantBuffer("LightBuffer", sizeof(LightBuffer), 4, PIPELINE_STAGE_PIXEL_SHADER, lightBuffer))
		{
			LOG(GELog, Error, "Failed to create light buffer");
			return false;
		}

		ConstantBuffer debugBuffer;
		if (!myRHI->CreateConstantBuffer("DebugBuffer", sizeof(DebugBuffer), 10, PIPELINE_STAGE_PIXEL_SHADER, debugBuffer))
		{
			LOG(GELog, Error, "Failed to create debug buffer");
			return false;
		}
		ConstantBuffer postProcessBuffer;
		if (!myRHI->CreateConstantBuffer("PostProcessBuffer", sizeof(PostProcessBuffer), 5, PIPELINE_STAGE_PIXEL_SHADER, postProcessBuffer))
		{
			LOG(GELog, Error, "Failed to create post process buffer");
			return false;
		}
		myConstantBuffers.emplace(ConstantBufferType::FrameBuffer, std::move(frameBuffer));
		myConstantBuffers.emplace(ConstantBufferType::ObjectBuffer, std::move(objectBuffer));
		myConstantBuffers.emplace(ConstantBufferType::MaterialBuffer, std::move(materialBuffer));
		myConstantBuffers.emplace(ConstantBufferType::LightBuffer, std::move(lightBuffer));
		myConstantBuffers.emplace(ConstantBufferType::DebugBuffer, std::move(debugBuffer));
		myConstantBuffers.emplace(ConstantBufferType::PostProcessBuffer, std::move(postProcessBuffer));
		InitPostProcessBuffer();

	return true;
}

void GraphicsEngine::Update()
{

}


bool GraphicsEngine::PrepareParticleEmitter(ParticleEmitter& anEmitter)
{
	if (!anEmitter.GetVertexBuffer()->Buffer)
	{
		return myRHI->CreateVertexBuffer(anEmitter.GetVertexBuffer()->Name, anEmitter.GetParticleVertices(), anEmitter.GetVertexBuffer()->Buffer, true);
	}
	else
	{
		return myRHI->UpdateVertexBuffer(*anEmitter.GetVertexBuffer(), anEmitter.GetParticleVertices());
	}
}


bool GraphicsEngine::ClearTextureResourceSlot(unsigned aPipelineStages, unsigned aSlot) const
{
	return myRHI->ClearTextureResourceSlot(aPipelineStages, aSlot);
}


bool GraphicsEngine::LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, const size_t& aTextureDataSize, TextureAsset& outTexture) const
{
	return myRHI->LoadTexture(aName, aTextureDataPtr, aTextureDataSize, outTexture);
}

bool GraphicsEngine::LoadTexture(const std::filesystem::path& aFilePath, TextureAsset& outTexture) const
{
	std::ifstream file(aFilePath, std::ios::binary);

	file.seekg(0, std::ios::end);
	const std::streamoff fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	if (fileSize < 0)
	{
#ifdef _DEBUG
		throw("Invalid Filepath in LoadTexture");
#endif
		return false;
	}

	std::vector<uint8_t> fileData;
	fileData.resize(fileSize);
	file.read(reinterpret_cast<char*>(fileData.data()), fileSize);
	file.close();

	return myRHI->LoadTexture(aFilePath.string(), fileData.data(), fileData.size(), outTexture);
}

bool GraphicsEngine::CreateTexture(const std::filesystem::path& aName, unsigned aWidth, unsigned aHeight, RHITextureFormat aFormat,
	TextureAsset* outTexture, bool aStaging, bool aShaderResource, bool aRenderTarget, bool aCpuAccessRead, bool aCpuAccessWrite) const
{
	myRHI->CreateTexture(aName.string(), aWidth, aHeight, aFormat, outTexture, aStaging, aShaderResource, aRenderTarget, aCpuAccessRead, aCpuAccessWrite);
	return true;
}

bool GraphicsEngine::CreateSRV(TextureAsset* outTexture, std::string_view aName) const
{
	myRHI->CreateSRV(outTexture, aName);
	return true;
}



std::shared_ptr<MaterialAsset> GraphicsEngine::GetDefaultMaterial() const
{
	return myDefaultMaterial;
}

void GraphicsEngine::BeginEvent(std::string_view aEvent) const
{
	myRHI->BeginEvent(aEvent);
}

void GraphicsEngine::EndEvent() const
{
	myRHI->EndEvent();
}

void GraphicsEngine::SetMarker(std::string_view aMarker) const
{
	myRHI->SetMarker(aMarker);
}

void GraphicsEngine::DrawQuad(const CU::Vector2f& aSize)
{
	//This should be moved into a command instead.
	//myDirLightPSO->SamplerStates[15] = myRHI->GetSamplerState("ShadowSamplerState");
	if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 100, *myDirLightShadowMap))
	{
		LOG(GELog, Error, "Couldn't set shadow map on slot {}", 100);
	}
	if (!SetGBufferResource())
	{
		LOG(GELog, Error, "Couldn't set GBuffer as resource");
	}
	myRHI->DrawQuad(aSize);
	if (!ClearGBuffer())
	{
		LOG(GELog, Error, "Couldn't clear GBuffer");
	}
	if (!ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 100))
	{
		LOG(GELog, Error, "Failed to clear dirlight shadow map on slot 100");
	}
}


void GraphicsEngine::RenderMesh(const MeshAsset& aMesh, const std::vector<std::shared_ptr<MaterialAsset>>& someMaterials) const
{
	if (myCurrentPSO->DepthStencil != myDirLightShadowMap)
	{
		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 100, *myDirLightShadowMap))
		{
			LOG(GELog, Error, "Couldn't set dirlightshadow map on slot {}", 100);
		}
	}
	else if (myCurrentPSO->DepthStencil != myPointLightShadowMaps[0])
	{
		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 101, *myPointLightShadowMaps[0]))
		{
			LOG(GELog, Error, "Couldn't set pointlightshadow map on slot {}", 101);
		}
	}
	else if (myCurrentPSO->DepthStencil != mySpotLightShadowMaps[0])
	{
		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 105, *mySpotLightShadowMaps[0]))
		{
			LOG(GELog, Error, "Couldn't set spotlightshadow map on slot {}", 105);
		}
	}

	for (size_t index = 0; index < aMesh.GetElements().size(); index++)
	{
		myRHI->SetVertexBuffer(aMesh.GetVertexBuffer(index), myCurrentVertexShader->VertexStride, 0);
		myRHI->SetIndexBuffer(aMesh.GetIndexBuffer(index));
		myRHI->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		UINT materialIndex = aMesh.GetElements()[index].MaterialIndex;
		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 0, *someMaterials[materialIndex]->GetAlbedoTexture()))
		{
			LOG(GELog, Error, "Couldn't set albedo texture on slot 0");
		}
		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 1, *someMaterials[materialIndex]->GetNormalTexture()))
		{
			LOG(GELog, Error, "Couldn't set normal texture on slot 1");

		}
		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 2, *someMaterials[materialIndex]->GetMaterialTexture()))
		{
			LOG(GELog, Error, "Couldn't set material texture on slot 2");
		}
		if (someMaterials[materialIndex]->GetFXTexture())
		{
			if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 3, *someMaterials[materialIndex]->GetFXTexture()))
			{
				LOG(GELog, Error, "Couldn't set fx texture on slot 3");
			}
		}
		else
		{
			if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 3, *myDefaultFXTexture))
			{
				LOG(GELog, Error, "Couldn't set fx texture on slot 3");
			}
		}
		myRHI->DrawIndexed(aMesh.GetElements()[index].IndexOffset, aMesh.GetElements()[index].NumIndices);
		
	}
	

	if (!myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 0))
	{
		LOG(GELog, Error, "Failed to clear texture resource slot");
	}
	if (!myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 1))
	{
		LOG(GELog, Error, "Failed to clear texture resource slot");
	}
	if (!myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 2))
	{
		LOG(GELog, Error, "Failed to clear texture resource slot");
	}
	if (!myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 3))
	{
		LOG(GELog, Error, "Failed to clear texture resource slot");
	}

	if (myCurrentPSO->DepthStencil != myDirLightShadowMap)
	{
		if (!myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 100))
		{
			LOG(GELog, Error, "Failed to clear texture resource slot");
		}
	}
}

void GraphicsEngine::RenderSprite2D(const SpriteAsset& anElement) const
{
	myRHI->SetIndexBuffer(nullptr);
	myRHI->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 0);
	SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 0, *anElement.GetTexture());
	myRHI->SetVertexBuffer(anElement.GetVertexBuffer(), myCurrentVertexShader->VertexStride, 0);
	myRHI->Draw(4);
}

void GraphicsEngine::RenderAudioUI(unsigned aStartIndex, unsigned anIndexCount)
{
	myRHI->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	myRHI->DrawIndexed(aStartIndex, anIndexCount);
}


void GraphicsEngine::RenderInstancedMesh(const MeshAsset& aMesh, const std::vector<std::shared_ptr<MaterialAsset>>& someMaterials, const InstanceData& anInstanceData) const
{

	if (myCurrentPSO->DepthStencil != myDirLightShadowMap)
	{
		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 100, *myDirLightShadowMap))
		{
			LOG(GELog, Error, "Couldn't set dirlightshadow map on slot {}", 100);
		}
	}
	else if (myCurrentPSO->DepthStencil != myPointLightShadowMaps[0])
	{
		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 101, *myPointLightShadowMaps[0]))
		{
			LOG(GELog, Error, "Couldn't set pointlightshadow map on slot {}", 101);
		}
	}
	else if (myCurrentPSO->DepthStencil != mySpotLightShadowMaps[0])
	{
		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 105, *mySpotLightShadowMaps[0]))
		{
			LOG(GELog, Error, "Couldn't set spotlightshadow map on slot {}", 105);
		}
	}

	for (size_t index = 0; index < aMesh.GetElements().size(); index++)
	{
		//myRHI->SetVertexBuffer(aMesh.GetVertexBuffer(index), myCurrentVertexShader->VertexStride, 0);
		myRHI->SetVertexBuffers(aMesh.GetVertexBuffer(index), anInstanceData.GetInstanceBuffer(), sizeof(Vertex), sizeof(CU::Matrix4x4<float>));
		myRHI->SetIndexBuffer(aMesh.GetIndexBuffer(index));
		myRHI->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		UINT materialIndex = aMesh.GetElements()[index].MaterialIndex;
		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 0, *someMaterials[materialIndex]->GetAlbedoTexture()))
		{
			LOG(GELog, Error, "Couldn't set albedo texture on slot 0");
		}
		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 1, *someMaterials[materialIndex]->GetNormalTexture()))
		{
			LOG(GELog, Error, "Couldn't set normal texture on slot 1");

		}
		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 2, *someMaterials[materialIndex]->GetMaterialTexture()))
		{
			LOG(GELog, Error, "Couldn't set material texture on slot 2");
		}
		if (someMaterials[materialIndex]->GetFXTexture())
		{
			if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 3, *someMaterials[materialIndex]->GetFXTexture()))
			{
				LOG(GELog, Error, "Couldn't set fx texture on slot 3");
			}
		}
		else
		{
			if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 3, *myDefaultFXTexture))
			{
				LOG(GELog, Error, "Couldn't set fx texture on slot 3");
			}
		}
		myRHI->DrawIndexedInstanced(aMesh.GetElements()[index].NumIndices, static_cast<unsigned>(anInstanceData.GetRelativeTransforms().size()));

	}


	if (!myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 0))
	{
		LOG(GELog, Error, "Failed to clear texture resource slot");
	}
	if (!myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 1))
	{
		LOG(GELog, Error, "Failed to clear texture resource slot");
	}
	if (!myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 2))
	{
		LOG(GELog, Error, "Failed to clear texture resource slot");
	}
	if (!myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 3))
	{
		LOG(GELog, Error, "Failed to clear texture resource slot");
	}

	if (myCurrentPSO->DepthStencil != myDirLightShadowMap)
	{
		if (!myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 100))
		{
			LOG(GELog, Error, "Failed to clear texture resource slot");
		}
	}
}

void GraphicsEngine::RenderPoints(const unsigned aParticleCount)
{
	myRHI->SetPrimitiveTopology(static_cast<unsigned>(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST));
	myRHI->Draw(aParticleCount);
}

void GraphicsEngine::RenderDebugLines(const DebugLineObject& aDebugLineObject)
{
	myRHI->SetVertexBuffer(aDebugLineObject.GetVertexBuffer(), myCurrentVertexShader->VertexStride, 0);
	myRHI->SetIndexBuffer(aDebugLineObject.GetIndexBuffer());
	myRHI->SetPrimitiveTopology(aDebugLineObject.GetPrimitiveTopology());

	myRHI->DrawIndexed(0, aDebugLineObject.GetNumIndices());
}


void GraphicsEngine::ConfigureInputAssembler(unsigned aTopology, const ComPtr<ID3D11Buffer>& aVxBuffer, const ComPtr<ID3D11Buffer>& anIxBuffer, unsigned aVertexStride, const ComPtr<ID3D11InputLayout>& anInputLayout)
{
	myRHI->ConfigureInputAssembler(aTopology, aVxBuffer, anIxBuffer, aVertexStride, anInputLayout);
}

bool GraphicsEngine::SetTextureResource(unsigned aPipelineStages, unsigned aSlot, const TextureAsset& aTexture) const
{
	myRHI->SetTextureResource(aPipelineStages, aSlot, aTexture);
	return true;
}

void GraphicsEngine::SetPixelShader(const std::string_view& aShaderName)
{
	if (aShaderName == "Empty" || aShaderName == "None")
	{
		myRHI->SetPixelShader(nullptr);
		myCurrentPixelShader = nullptr;
		return;
	}
	auto shader = myShaders[myShaderMap[aShaderName]];
	if (!shader)
	{
		LOG(GELog, Error, "{} does not exist, {}", aShaderName);
		return;
	}
	if (shader->type != ShaderType::PixelShader)
	{
		LOG(GELog, Error, "The pixel shader can't be set as it isn't a pixel shader");
	}
	myRHI->SetPixelShader(shader);
	myCurrentPixelShader = shader;
}

void GraphicsEngine::SetVertexShader(const std::string_view& aShaderName)
{
	auto& shader = myShaders[myShaderMap[aShaderName]];
	if (!shader)
	{
#ifndef _RELEASE
		LOG(GELog, Error, "{} does not exist, {}", aShaderName);
		return;
#endif
	}
	if (shader->type != ShaderType::VertexShader)
	{
		LOG(GELog, Error, "The vertex shader can't be set as it isn't a vertex shader");
	}
	std::shared_ptr<VertexShader> vShader = std::dynamic_pointer_cast<VertexShader>(shader);
	if (vShader == nullptr)
	{
		LOG(GELog, Error, "The vertex shader can't be set as it isn't a vertex shader");
	}
	myRHI->SetInputLayout(vShader->inputLayout);
	myRHI->SetVertexShader(vShader);
	myCurrentVertexShader = vShader;
}

void GraphicsEngine::SetGeometryShader(const std::string_view& aShaderName)
{
	if (aShaderName.empty() || aShaderName == "None" || aShaderName == "Empty")
	{
		myRHI->SetGeometryShader(nullptr);
		myCurrentGeometryShader = nullptr;
		return;
	}

	auto shader = myShaders[myShaderMap[aShaderName]];
	if (!shader)
	{
		LOG(GELog, Error, "{} does not exist, {}", aShaderName);
		return;
	}
	if (shader->type != ShaderType::GeometryShader)
	{
		LOG(GELog, Error, "The geometry shader can't be set as it isn't a vertex shader");
	}
	std::shared_ptr<GeometryShader> gShader = std::dynamic_pointer_cast<GeometryShader>(shader);
	if (gShader == nullptr)
	{
		LOG(GELog, Error, "The geometry shader can't be set as it isn't a vertex shader");
	}
	//myRHI->SetInputLayout(gShader->inputLayout);
	myRHI->SetGeometryShader(gShader);
	myCurrentGeometryShader = gShader;
}

void GraphicsEngine::SetShadowMap(ShadowMaps aShadowMap, unsigned anIndex)
{
	switch (aShadowMap)
	{
	case ShadowMaps::DirLightShadowMap:
		//myRHI->SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 105, *myShadowPSO.DepthStencil);
		myShadowPSO->DepthStencil = myDirLightShadowMap;
		return;
	case ShadowMaps::PointLightShadowMap:
		//myRHI->SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 100, *myShadowPSO.DepthStencil);
		myShadowPSO->DepthStencil = myPointLightShadowMaps[anIndex];
		return;
	case ShadowMaps::SpotLightShadowMap:
		//myRHI->SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 101, *myShadowPSO.DepthStencil);
		myShadowPSO->DepthStencil = mySpotLightShadowMaps[anIndex];
		return;
	default:
		return;
	}
}

void GraphicsEngine::SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> aVertexBuffer)
{
	myRHI->SetVertexBuffer(aVertexBuffer, myCurrentVertexShader->VertexStride, 0);
}

void GraphicsEngine::SetIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> anIndexBuffer)
{
	myRHI->SetIndexBuffer(anIndexBuffer);
}


void GraphicsEngine::ChangePipelineState(const unsigned aNewPipelineState)
{
	switch (static_cast<PipelineStates>(aNewPipelineState))
	{
	case PipelineStates::DirlightShadowMapping:
		SetShadowMap(ShadowMaps::DirLightShadowMap);
		ChangePipelineState(myShadowPSO);
		break;
	case PipelineStates::PointlightShadowMapping:
		myRHI->ChangeDepthStencil(nullptr);
		myRHI->SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 100, *myShadowPSO->DepthStencil);
		SetShadowMap(ShadowMaps::PointLightShadowMap, 0);
		myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 101);
		ChangePipelineState(myShadowPSO);
		break;
	case PipelineStates::SpotlightShadowMapping:
		myRHI->ChangeDepthStencil(nullptr);
		myRHI->SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 101, *myShadowPSO->DepthStencil);
		SetShadowMap(ShadowMaps::SpotLightShadowMap, 0);
		myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 105);
		ChangePipelineState(myShadowPSO);
		break;
	case PipelineStates::DeferredRendering:
		ChangePipelineState(myDeferredPSO);
		break;
	case PipelineStates::DirlightRendering:
		ChangePipelineState(myDirLightPSO);
		break;
	case PipelineStates::PointlightRendering:
		ChangePipelineState(myPointLightPSO);
		break;
	case PipelineStates::SpotlightRendering:
		ChangePipelineState(mySpotLightPSO);
		break;
	case PipelineStates::ForwardRendering:
		ChangePipelineState(myForwardPSO);
		break;
	case PipelineStates::SpriteRendering:
		ChangePipelineState(mySpritePSO);
		break;
	case PipelineStates::ParticleRendering:
		ChangePipelineState(myParticlePSO);
		break;
	case PipelineStates::Tonemap:
		ChangePipelineState(myTonemapPSO);
		break;
	case PipelineStates::Luminance:
		ChangePipelineState(myLuminancePSO);
		break;
	case PipelineStates::GaussianV:
		ChangePipelineState(myGaussianVPSO);
		break;
	case PipelineStates::GaussianH:
		ChangePipelineState(myGaussianHPSO);
		break;
	case PipelineStates::Resampling:
		ChangePipelineState(myResamplingPSO);
		break;
	case PipelineStates::Bloom:
		ChangePipelineState(myBloomPSO);
		break;
	case PipelineStates::SSAO:
		ChangePipelineState(mySSAOPSO);
		break;
	default:
		break;
	}
}

void GraphicsEngine::ChangeRenderTarget(const std::shared_ptr<TextureAsset>& aRenderTarget) const
{
	myRHI->SetRenderTarget(aRenderTarget);
}

void GraphicsEngine::ClearBackBuffer() const
{
	myRHI->ClearBackBuffer();
}

void GraphicsEngine::ClearRenderTarget()
{
	if (myCurrentPSO->RenderTarget[0])
	{
		myRHI->ClearRenderTarget(myCurrentPSO->RenderTarget[0].get());
	}
	else
	{
		myRHI->SetRenderTarget(nullptr);
	}
}

void GraphicsEngine::ClearRenderTarget(const std::shared_ptr<TextureAsset>& aRenderTarget) const
{
	if (!aRenderTarget->GetRTV())
	{
		LOG(GELog, Error, "Cannot clear render target. RTV is null");
	}
	myRHI->ClearRenderTarget(aRenderTarget.get());
}


void GraphicsEngine::CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& aOutIxBuffer)
{
	myRHI->CreateIndexBuffer(aName, aIndexList, aOutIxBuffer);
}

const std::vector<std::shared_ptr<MaterialAsset>>& GraphicsEngine::GetDefaultMaterials() const
{
	return myDefaultMaterials;
}

HWND GraphicsEngine::GetWindowHandle() const
{
	return myWindowHandler->GetWindowHandle();
}

SIZE GraphicsEngine::GetWindowSize() const
{
	return myWindowHandler->GetWindowSize();
}

CU::Vector2f GraphicsEngine::GetWindowSizeAsVector() const
{
	return myRHI->GetBackBuffer()->GetSize();
}

bool GraphicsEngine::CreateForwardPSO()
{
	myForwardPSO = std::make_shared<PipelineStateObject>();
	myForwardPSO->RenderTargetCount = 1;
	myForwardPSO->Type = PSOType::Forward;

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	myRHI->CreateBlendState("ForwardBlendState", blendDesc);
	myForwardPSO->BlendState = myRHI->GetBlendState("ForwardBlendState");

	D3D11_SAMPLER_DESC defaultSamplerDesc = {};
	defaultSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	defaultSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	defaultSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	defaultSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	defaultSamplerDesc.MipLODBias = 0.0f;
	defaultSamplerDesc.MaxAnisotropy = 1;
	defaultSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	defaultSamplerDesc.BorderColor[0] = 1.0f;
	defaultSamplerDesc.BorderColor[1] = 1.0f;
	defaultSamplerDesc.BorderColor[2] = 1.0f;
	defaultSamplerDesc.BorderColor[3] = 1.0f;
	defaultSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	defaultSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	myRHI->CreateSamplerState("DefaultSamplerState", defaultSamplerDesc);
	myForwardPSO->SamplerStates[0] = myRHI->GetSamplerState("DefaultSamplerState");


	myForwardPSO->RenderTarget[0] = myHDRBuffer;
	myForwardPSO->ClearRenderTarget[0] = false;

	myForwardPSO->DepthStencil = myRHI->GetDepthBuffer();
	myForwardPSO->ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateShadowPSO()
{
	myShadowPSO = std::make_shared<PipelineStateObject>();
	myShadowPSO->Type = PSOType::Shadow;
	myShadowPSO->BlendState = nullptr;
	myShadowPSO->RenderTargetCount = 0;
	//Shadow PSO
	D3D11_SAMPLER_DESC shadowSamplerDesc;
	shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.MipLODBias = 0.0f;
	shadowSamplerDesc.MaxAnisotropy = 1;
	shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	shadowSamplerDesc.BorderColor[0] = 0.0f;
	shadowSamplerDesc.BorderColor[1] = 0.0f;
	shadowSamplerDesc.BorderColor[2] = 0.0f;
	shadowSamplerDesc.BorderColor[3] = 0.0f;
	shadowSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	myRHI->CreateSamplerState("ShadowSamplerState", shadowSamplerDesc);
	myShadowPSO->SamplerStates[15] = myRHI->GetSamplerState("ShadowSamplerState");
	myShadowPSO->RenderTarget[0] = nullptr;
	myShadowPSO->ClearRenderTarget[0] = false;
	myShadowPSO->DepthStencil = myDirLightShadowMap;
	myShadowPSO->ClearDepthStencil = true;

	return true;
}

bool GraphicsEngine::CreatePointShadowPSO()
{
	myPointShadowPSO = std::make_shared<PipelineStateObject>();
	myPointShadowPSO->Type = PSOType::PointShadow;
	myPointShadowPSO->BlendState = nullptr;
	myPointShadowPSO->RenderTargetCount = 0;
	myPointShadowPSO->SamplerStates[15] = myRHI->GetSamplerState("ShadowSamplerState");
	myPointShadowPSO->RenderTarget[0] = nullptr;
	myPointShadowPSO->ClearRenderTarget[0] = false;
	myPointShadowPSO->DepthStencil = myPointLightShadowMaps[0];
	myPointShadowPSO->ClearDepthStencil = true;
	return true;
}

bool GraphicsEngine::CreateSpotShadowPSO()
{
	mySpotShadowPSO = std::make_shared<PipelineStateObject>();
	mySpotShadowPSO->Type = PSOType::SpotShadow;
	mySpotShadowPSO->BlendState = nullptr;
	mySpotShadowPSO->RenderTargetCount = 0;
	mySpotShadowPSO->SamplerStates[15] = myRHI->GetSamplerState("ShadowSamplerState");
	mySpotShadowPSO->RenderTarget[0] = nullptr;
	mySpotShadowPSO->ClearRenderTarget[0] = false;
	mySpotShadowPSO->DepthStencil = mySpotLightShadowMaps[0];
	mySpotShadowPSO->ClearDepthStencil = true;
	return true;
}

bool GraphicsEngine::CreateDeferredPSO()
{
	myDeferredPSO = std::make_shared<PipelineStateObject>();
	myDeferredPSO->Type = PSOType::Deferred;
	myDeferredPSO->BlendState = nullptr;
	myDeferredPSO->RenderTargetCount = 5;
	myDeferredPSO->SamplerStates[0] = myRHI->GetSamplerState("DefaultSamplerState");
	myDeferredPSO->RenderTarget[0] = myGBuffer.Albedo;
	myDeferredPSO->RenderTarget[1] = myGBuffer.WorldNormal;
	myDeferredPSO->RenderTarget[2] = myGBuffer.Material;
	myDeferredPSO->RenderTarget[3] = myGBuffer.WorldPosition;
	myDeferredPSO->RenderTarget[4] = myGBuffer.FXTexture;

	for (int i = 0; i < myDeferredPSO->RenderTargetCount; i++)
	{
		myDeferredPSO->ClearRenderTarget[i] = true;
	}
	myDeferredPSO->DepthStencil = myRHI->GetDepthBuffer();
	myDeferredPSO->ClearDepthStencil = true;
	return true;

}

bool GraphicsEngine::CreateDirLightPSO()
{
	CreateAdditiveBlendState();
	myDirLightPSO = std::make_shared<PipelineStateObject>();
	myDirLightPSO->Type = PSOType::DirLight;
	myDirLightPSO->BlendState = myRHI->GetBlendState("AdditiveBlendState");
	myDirLightPSO->RenderTargetCount = 1;
	myDirLightPSO->SamplerStates[0] = myRHI->GetSamplerState("DefaultSamplerState");
	myDirLightPSO->RenderTarget[0] = myHDRBuffer;
	myDirLightPSO->ClearRenderTarget[0] = true;
	myDirLightPSO->DepthStencil = nullptr;
	myDirLightPSO->ClearDepthStencil = false;

	return true;
}

bool GraphicsEngine::CreatePointLightPSO()
{
	myPointLightPSO = std::make_shared<PipelineStateObject>();
	myPointLightPSO->Type = PSOType::PointLight;
	myPointLightPSO->BlendState = myRHI->GetBlendState("AdditiveBlendState");
	myPointLightPSO->RenderTargetCount = 1;
	myPointLightPSO->SamplerStates[0] = myRHI->GetSamplerState("DefaultSamplerState");
	myPointLightPSO->RenderTarget[0] = myHDRBuffer;
	myPointLightPSO->ClearRenderTarget[0] = false;
	myPointLightPSO->DepthStencil = nullptr;
	myPointLightPSO->ClearDepthStencil = false;
	return true;;
}

bool GraphicsEngine::CreateSpotLightPSO()
{
	mySpotLightPSO = std::make_shared<PipelineStateObject>();
	mySpotLightPSO->Type = PSOType::SpotLight;

	mySpotLightPSO->BlendState = myRHI->GetBlendState("AdditiveBlendState");
	mySpotLightPSO->RenderTargetCount = 1;
	mySpotLightPSO->SamplerStates[0] = myRHI->GetSamplerState("DefaultSamplerState");


	mySpotLightPSO->RenderTarget[0] = myHDRBuffer;
	mySpotLightPSO->ClearRenderTarget[0] = false;

	mySpotLightPSO->DepthStencil = nullptr;
	mySpotLightPSO->ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateSpritePSO()
{
	mySpritePSO = std::make_shared<PipelineStateObject>();
	mySpritePSO->Type = PSOType::Sprite;
	mySpritePSO->RenderTargetCount = 1;

	D3D11_SAMPLER_DESC spriteSamplerDesc = {};
	spriteSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	spriteSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	spriteSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	spriteSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	spriteSamplerDesc.MipLODBias = 0.0f;
	spriteSamplerDesc.MaxAnisotropy = 1;
	spriteSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	spriteSamplerDesc.BorderColor[0] = 1.0f;
	spriteSamplerDesc.BorderColor[1] = 1.0f;
	spriteSamplerDesc.BorderColor[2] = 1.0f;
	spriteSamplerDesc.BorderColor[3] = 1.0f;
	spriteSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	spriteSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	myRHI->CreateSamplerState("SpriteSamplerState", spriteSamplerDesc);

	mySpritePSO->SamplerStates[0] = myRHI->GetSamplerState("SpriteSamplerState");

	D3D11_BLEND_DESC blendStateDescription = {};
	blendStateDescription = {};
	blendStateDescription.AlphaToCoverageEnable = false;
	blendStateDescription.IndependentBlendEnable = false;
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	myRHI->CreateBlendState("SpriteBlendState", blendStateDescription);
	mySpritePSO->BlendState = myRHI->GetBlendState("SpriteBlendState");

	mySpritePSO->RenderTarget[0] = myRHI->GetBackBuffer();
	mySpritePSO->ClearRenderTarget[0] = true;

	mySpritePSO->DepthStencil = nullptr;
	mySpritePSO->ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateTonemapPSO()
{
	myTonemapPSO = std::make_shared<PipelineStateObject>();
	myTonemapPSO->Type = PSOType::Tonemap;
	myTonemapPSO->BlendState = nullptr;
	myTonemapPSO->RenderTargetCount = 1;
	myTonemapPSO->SamplerStates[0] = myRHI->GetSamplerState("DefaultSamplerState");

	myTonemapPSO->RenderTarget[0] = myLDRBuffer;
	myTonemapPSO->ClearRenderTarget[0] = false;

	myTonemapPSO->DepthStencil = nullptr;
	myTonemapPSO->ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateResamplingPSO()
{
	myResamplingPSO = std::make_shared<PipelineStateObject>();
	myResamplingPSO->Type = PSOType::Resampling;
	myResamplingPSO->BlendState = nullptr;
	myResamplingPSO->RenderTargetCount = 1;

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	myRHI->CreateSamplerState("DefaultClampSamplerState", samplerDesc);

	myResamplingPSO->SamplerStates[1] = myRHI->GetSamplerState("DefaultClampSamplerState");
	myDirLightPSO->SamplerStates[1] = myRHI->GetSamplerState("DefaultClampSamplerState");

	myResamplingPSO->RenderTarget[0] = nullptr;
	myResamplingPSO->ClearRenderTarget[0] = false;

	myResamplingPSO->DepthStencil = nullptr;
	myResamplingPSO->ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateLuminancePSO()
{
	myLuminancePSO = std::make_shared<PipelineStateObject>();
	myLuminancePSO->Type = PSOType::Luminance;
	myLuminancePSO->BlendState = nullptr;
	myLuminancePSO->RenderTargetCount = 1;
	myLuminancePSO->SamplerStates[0] = myRHI->GetSamplerState("DefaultSamplerState");
	myLuminancePSO->RenderTarget[0] = myLuminanceBuffer;
	myLuminancePSO->ClearRenderTarget[0] = false;
	myLuminancePSO->DepthStencil = nullptr;
	myLuminancePSO->ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateGaussianHPSO()
{
	myGaussianHPSO = std::make_shared<PipelineStateObject>();
	myGaussianHPSO->Type = PSOType::GaussianH;
	myGaussianHPSO->BlendState = nullptr;
	myGaussianHPSO->RenderTargetCount = 1;
	myGaussianHPSO->SamplerStates[1] = myRHI->GetSamplerState("DefaultClampSamplerState");
	myGaussianHPSO->RenderTarget[0] = myEighthScreenBufferB;
	myGaussianHPSO->ClearRenderTarget[0] = false;
	myGaussianHPSO->DepthStencil = nullptr;
	myGaussianHPSO->ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateGaussianVPSO()
{
	myGaussianVPSO = std::make_shared<PipelineStateObject>();
	myGaussianVPSO->Type = PSOType::GaussianV;
	myGaussianVPSO->BlendState = nullptr;
	myGaussianVPSO->RenderTargetCount = 1;
	myGaussianVPSO->SamplerStates[1] = myRHI->GetSamplerState("DefaultClampSamplerState");
	myGaussianVPSO->RenderTarget[0] = myEighthScreenBufferA;
	myGaussianVPSO->ClearRenderTarget[0] = false;
	myGaussianVPSO->DepthStencil = nullptr;
	myGaussianVPSO->ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateBloomPSO()
{
	myBloomPSO = std::make_shared<PipelineStateObject>();
	myBloomPSO->Type = PSOType::Bloom;

	D3D11_BLEND_DESC alphaBlendDesc = {};
	alphaBlendDesc.RenderTarget[0].BlendEnable = true;
	alphaBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alphaBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	alphaBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	alphaBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	alphaBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	alphaBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	alphaBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (!myRHI->CreateBlendState("AlphaBlend", alphaBlendDesc))
	{
		LOG(GELog, Error, "Couldn't create alpha blend desc");
		return false;
	}
	myBloomPSO->BlendState = myRHI->GetBlendState("AlphaBlend");
	myBloomPSO->RenderTargetCount = 1;
	myBloomPSO->SamplerStates[0] = myRHI->GetSamplerState("DefaultSamplerState");
	myBloomPSO->RenderTarget[0] = myRHI->GetBackBuffer();
	myBloomPSO->ClearRenderTarget[0] = false;
	myBloomPSO->DepthStencil = nullptr;
	myBloomPSO->ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateSSAOPSO()
{
	mySSAOPSO = std::make_shared<PipelineStateObject>();
	mySSAOPSO->Type = PSOType::SSAO;


	D3D11_SAMPLER_DESC pointClampSamplerDesc = {};
	pointClampSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	pointClampSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointClampSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointClampSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointClampSamplerDesc.MipLODBias = 0.0f;
	pointClampSamplerDesc.MaxAnisotropy = 1;
	pointClampSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	pointClampSamplerDesc.BorderColor[0] = 1.0f;
	pointClampSamplerDesc.BorderColor[1] = 1.0f;
	pointClampSamplerDesc.BorderColor[2] = 1.0f;
	pointClampSamplerDesc.BorderColor[3] = 1.0f;
	pointClampSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	pointClampSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	myRHI->CreateSamplerState("PointClampSamplerState", pointClampSamplerDesc);

	mySSAOPSO->SamplerStates[2] = myRHI->GetSamplerState("PointClampSamplerState");

	D3D11_SAMPLER_DESC pointWrapSamplerDesc = {};
	pointWrapSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	pointWrapSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	pointWrapSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	pointWrapSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pointWrapSamplerDesc.MipLODBias = 0.0f;
	pointWrapSamplerDesc.MaxAnisotropy = 1;
	pointWrapSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	pointWrapSamplerDesc.BorderColor[0] = 1.0f;
	pointWrapSamplerDesc.BorderColor[1] = 1.0f;
	pointWrapSamplerDesc.BorderColor[2] = 1.0f;
	pointWrapSamplerDesc.BorderColor[3] = 1.0f;
	pointWrapSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	pointWrapSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	myRHI->CreateSamplerState("PointWrapSamplerState", pointWrapSamplerDesc);

	mySSAOPSO->SamplerStates[3] = myRHI->GetSamplerState("PointWrapSamplerState");


	mySSAOPSO->BlendState = nullptr;
	mySSAOPSO->RenderTargetCount = 1;
	mySSAOPSO->RenderTarget[0] = nullptr;
	mySSAOPSO->ClearRenderTarget[0] = false;
	mySSAOPSO->DepthStencil = nullptr;
	mySSAOPSO->ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateParticlePSO()
{
	myParticlePSO = std::make_shared<PipelineStateObject>();
	myParticlePSO->Type = PSOType::Particle;

	myParticlePSO->SamplerStates[0] = myRHI->GetSamplerState("DefaultSamplerState");

	myParticlePSO->DepthStencilState = myRHI->GetDepthStencilState(DepthState::DS_ReadOnly);
	myParticlePSO->BlendState = myRHI->GetBlendState("AlphaBlend");
	myParticlePSO->RenderTargetCount = 1;
	myParticlePSO->RenderTarget[0] = myHDRBuffer;
	myParticlePSO->ClearRenderTarget[0] = false;
	myParticlePSO->DepthStencil = nullptr;
	myParticlePSO->ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateDirLightShadowMap(const std::string& aName)
{
	RECT clientRect = {};
	GetClientRect(myWindowHandler->GetWindowHandle(), &clientRect);
	CU::Vector2<unsigned> size = { 8000, 8000 };

	myDirLightShadowMap = std::make_shared<TextureAsset>();
	if (!myRHI->CreateShadowMap(aName, size, *myDirLightShadowMap))
	{
		return false;
	}
	return true;
}

void GraphicsEngine::CreatePointLightShadowMaps(const std::string& aName)
{
	RECT clientRect = {};
	GetClientRect(myWindowHandler->GetWindowHandle(), &clientRect);
	CU::Vector2<unsigned> size = { 8000, 8000 };

	for (auto& map : myPointLightShadowMaps)
	{
		map = std::make_shared<TextureAsset>();
	}

	if (!myRHI->CreateShadowMap(aName + "1", size, *myPointLightShadowMaps[0]))
	{
		LOG(GELog, Error, "Couldn't create PointLight shadow map 1");
	}
	if (!myRHI->CreateShadowMap(aName + "2", size, *myPointLightShadowMaps[1]))
	{
		LOG(GELog, Error, "Couldn't create PointLight shadow map 2");
	}
	if (!myRHI->CreateShadowMap(aName + "3", size, *myPointLightShadowMaps[2]))
	{
		LOG(GELog, Error, "Couldn't create PointLight shadow map 3");
	}
	if (!myRHI->CreateShadowMap(aName + "4", size, *myPointLightShadowMaps[3]))
	{
		LOG(GELog, Error, "Couldn't create PointLight shadow map 4");
	}
}

void GraphicsEngine::CreateSpotLightShadowMaps(const std::string& aName)
{
	RECT clientRect = {};
	GetClientRect(myWindowHandler->GetWindowHandle(), &clientRect);
	CU::Vector2<unsigned> size = { 8000, 8000 };

	for (auto& map : mySpotLightShadowMaps)
	{
		map = std::make_shared<TextureAsset>();
	}

	if (!myRHI->CreateShadowMap(aName + "1", size, *mySpotLightShadowMaps[0]))
	{
		LOG(GELog, Error, "Couldn't create Spotlight shadow map 1");
	}
	if (!myRHI->CreateShadowMap(aName + "2", size, *mySpotLightShadowMaps[1]))
	{
		LOG(GELog, Error, "Couldn't create Spotlight shadow map 2");
	}
	if (!myRHI->CreateShadowMap(aName + "3", size, *mySpotLightShadowMaps[2]))
	{
		LOG(GELog, Error, "Couldn't create Spotlight shadow map 3");
	}
	if (!myRHI->CreateShadowMap(aName + "4", size, *mySpotLightShadowMaps[3]))
	{
		LOG(GELog, Error, "Couldn't create Spotlight shadow map 4");
	}
}

bool GraphicsEngine::CreateLUTTexture()
{
	myLUTTexture = std::make_shared<TextureAsset>();
	if (!myRHI->CreateLUTTexture(*myLUTTexture, myShaders[myShaderMap["BRDF_PS"]], myShaders[myShaderMap["Quad_VS"]]))
	{
		return false;
	}
	myForwardPSO->SamplerStates[14] = myRHI->GetSamplerState("LUTSampler");
	return true;
}

bool GraphicsEngine::CreateGBuffer()
{
	myGBuffer.Albedo = std::make_shared<TextureAsset>();
	myGBuffer.WorldNormal = std::make_shared<TextureAsset>();
	myGBuffer.Material = std::make_shared<TextureAsset>();
	myGBuffer.WorldPosition = std::make_shared<TextureAsset>();
	myGBuffer.FXTexture = std::make_shared<TextureAsset>();

	if (!myRHI->CreateTexture("AlbedoTexture", 1904, 1041, RHITextureFormat::R8G8B8A8_UNORM, myGBuffer.Albedo.get(), false, true, true, false, false))
	{
		LOG(GELog,Error, "Couldn't create albedo texture for GBuffer");
		return false;
	}
	if (!myRHI->CreateTexture("WorldNormalTexture", 1904, 1041, RHITextureFormat::R16G16B16A16_SNORM, myGBuffer.WorldNormal.get(), false, true, true, false, false))
	{
		LOG(GELog, Error, "Couldn't create material texture for GBuffer");
		return false;
	}
	if (!myRHI->CreateTexture("MaterialTexture", 1904, 1041, RHITextureFormat::R8G8B8A8_UNORM, myGBuffer.Material.get(), false, true, true, false, false))
	{
		LOG(GELog, Error, "Couldn't create pixel normal texture for GBuffer");
		return false;
	}
	if (!myRHI->CreateTexture("WorldPositionTexture", 1904, 1041, RHITextureFormat::R32G32B32A32_Float, myGBuffer.WorldPosition.get(), false, true, true, false, false))
	{
		LOG(GELog, Error, "Couldn't create world position texture for GBuffer");
		return false;
	}
	if (!myRHI->CreateTexture("FXTexture", 1904, 1041, RHITextureFormat::R8G8B8A8_UNORM, myGBuffer.FXTexture.get(), false, true, true, false, false))
	{
		LOG(GELog, Error, "Couldn't create fx texture for GBuffer");
		return false;
	}

	return true;
}

bool GraphicsEngine::ClearGBuffer() const
{
	bool result = true;
	if (!myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 10))
	{
		LOG(GELog, Error, "Couldn't clear Gbuffer albedo");
		result = false;
	}
	if (!myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 11))
	{
		LOG(GELog, Error, "Couldn't clear Gbuffer normal");
		result = false;
	}
	if (!myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 12))
	{
		LOG(GELog, Error, "Couldn't clear Gbuffer material");
		result = false;
	}
	if (!myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 13))
	{
		LOG(GELog, Error, "Couldn't clear Gbuffer position");
		result = false;
	}
	if (!myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 14))
	{
		LOG(GELog, Error, "Couldn't clear Gbuffer Fx");
		result = false;
	}
	return result;
}

bool GraphicsEngine::SetGBufferResource() const
{
	if (!myRHI->SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 10, *myGBuffer.Albedo))
	{
		LOG(GELog, Error, "Couldn't set texture resource at slot 10");
		return false;
	}
	if (!myRHI->SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 11, *myGBuffer.WorldNormal))
	{
		LOG(GELog, Error, "Couldn't set texture resource at slot 11");
		return false;
	}
	if (!myRHI->SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 12, *myGBuffer.Material))
	{
		LOG(GELog, Error, "Couldn't set texture resource at slot 12");
		return false;
	}
	if (!myRHI->SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 13, *myGBuffer.WorldPosition))
	{
		LOG(GELog, Error, "Couldn't set texture resource at slot 13");
		return false;
	}
	if (!myRHI->SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 14, *myGBuffer.FXTexture))
	{
		LOG(GELog, Error, "Couldn't set texture resource at slot 14");
		return false;
	}
	return true;
}

bool GraphicsEngine::CreateAdditiveBlendState()
{
	D3D11_BLEND_DESC additiveBlendDesc = {};
	additiveBlendDesc.RenderTarget[0].BlendEnable = true;
	additiveBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	additiveBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	additiveBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	additiveBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	additiveBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	additiveBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	additiveBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (!myRHI->CreateBlendState("AdditiveBlendState", additiveBlendDesc))
	{
		LOG(GELog, Error, "Couldn't create additive blend desc");
		return false;
	}
	return true;
}

void GraphicsEngine::CreateIntermediateBuffers()
{
	SetVertexShader("Quad_VS");
	CU::Vector2<unsigned> size;
	size.x = static_cast<unsigned>(myRHI->GetViewPortSize().x);
	size.y = static_cast<unsigned>(myRHI->GetViewPortSize().y);

	myHDRBuffer = std::make_shared<TextureAsset>();
	myHDRBuffer->SetSize(CU::Vector2f{ static_cast<float>(size.x), static_cast<float>(size.y) });
	if (!myRHI->CreateTexture("Intermediate HDR Buffer", size.x, size.y, RHITextureFormat::R16G16B16A16_FLOAT, myHDRBuffer.get(), false, true, true, false, false))
	{
		LOG(GELog, Error, "Unable to create HDR intermediate buffer");
	}
	myLDRBuffer = std::make_shared<TextureAsset>();
	myLDRBuffer->SetSize(CU::Vector2f{ static_cast<float>(size.x), static_cast<float>(size.y) });
	if (!myRHI->CreateTexture("Intermediate LDR Buffer", size.x, size.y, RHITextureFormat::R16G16B16A16_FLOAT, myLDRBuffer.get(), false, true, true, false, false))
	{
		LOG(GELog, Error, "Unable to create intermediate LDR buffer");
	}
	myLuminanceBuffer = std::make_shared<TextureAsset>();
	myLuminanceBuffer->SetSize(CU::Vector2f{ static_cast<float>(size.x), static_cast<float>(size.y) });
	if (!myRHI->CreateTexture("Luminance Buffer", size.x, size.y, RHITextureFormat::R16G16B16A16_FLOAT, myLuminanceBuffer.get(), false, true, true, false, false))
	{
		LOG(GELog, Error, "Unable to create Luminance buffer");
	}

	mySSAOBuffer = std::make_shared<TextureAsset>();
	mySSAOBuffer->SetSize(CU::Vector2f{ static_cast<float>(size.x), static_cast<float>(size.y) });
	if (!myRHI->CreateTexture("SSAO Buffer", static_cast<unsigned>(size.x), static_cast<unsigned>(size.y), RHITextureFormat::R16G16B16A16_FLOAT, mySSAOBuffer.get(), false, true, true, false, false))
	{
		LOG(GELog, Error, "Unable to create SSAO Buffer");
	}

	size.x = size.x / 2;
	size.y = size.y / 2;
	myHalfScreenBuffer = std::make_shared<TextureAsset>();
	myHalfScreenBuffer->SetSize(CU::Vector2f{ static_cast<float>(size.x), static_cast<float>(size.y) });
	if (!myRHI->CreateTexture("Half Size Buffer", size.x, (size.y), RHITextureFormat::R8G8B8A8_UNORM, myHalfScreenBuffer.get(), false, true, true, false, false))
	{
		LOG(GELog, Error, "Unable to create Half Size Buffer");
	}
	myHalfScreenBufferB = std::make_shared<TextureAsset>();
	myHalfScreenBufferB->SetSize(CU::Vector2f{ static_cast<float>(size.x), static_cast<float>(size.y) });
	if (!myRHI->CreateTexture("Half Size Buffer B", size.x, (size.y), RHITextureFormat::R8G8B8A8_UNORM, myHalfScreenBufferB.get(), false, true, true, false, false))
	{
		LOG(GELog, Error, "Unable to create Half Size Buffer B");
	}
	size.x = size.x / 2;
	size.y = size.y / 2;

	myQuarterScreenBuffer = std::make_shared<TextureAsset>();
	myQuarterScreenBuffer->SetSize(CU::Vector2f{ static_cast<float>(size.x), static_cast<float>(size.y) });
	if (!myRHI->CreateTexture("Quarter Size Buffer", size.x, size.y, RHITextureFormat::R8G8B8A8_UNORM, myQuarterScreenBuffer.get(), false, true, true, false, false))
	{
		LOG(GELog, Error, "Unable to create Quarter Size Buffer");
	}

	size.x = size.x / 2;
	size.y = size.y / 2;	
	myEighthScreenBufferA = std::make_shared<TextureAsset>();
	myEighthScreenBufferA->SetSize(CU::Vector2f{ static_cast<float>(size.x), static_cast<float>(size.y) });
	if (!myRHI->CreateTexture("Eight Size Buffer A", size.x, size.y, RHITextureFormat::R8G8B8A8_UNORM, myEighthScreenBufferA.get(), false, true, true, false, false))
	{
		LOG(GELog, Error, "Unable to create Eight Size Buffer A");
	}

	myEighthScreenBufferB = std::make_shared<TextureAsset>();
	myEighthScreenBufferB->SetSize(CU::Vector2f{ static_cast<float>(size.x), static_cast<float>(size.y) });
	if (!myRHI->CreateTexture("Eight Size Buffer B", static_cast<unsigned>(size.x), static_cast<unsigned>(size.y), RHITextureFormat::R8G8B8A8_UNORM, myEighthScreenBufferB.get(), false, true, true, false, false))
	{
		LOG(GELog, Error, "Unable to create Eight Size Buffer B");
	}

}

bool GraphicsEngine::CreateDefaultMaterials()
{
	myDefaultAlbedoTexture = std::make_shared<TextureAsset>();
	if (!myRHI->LoadTextureFromMemory(
		myDefaultAlbedoTexture.get(),
		L"Default Albedo",
		BuiltIn_Default_C_ByteCode,
		sizeof(BuiltIn_Default_C_ByteCode)))
	{
		return false;
	}
	myDefaultNormalTexture = std::make_shared<TextureAsset>();
	if (!myRHI->LoadTextureFromMemory(
		myDefaultNormalTexture.get(),
		L"Default Normal",
		BuiltIn_Default_N_ByteCode,
		sizeof(BuiltIn_Default_N_ByteCode)))
	{
		return false;
	}
	myDefaultMaterialTexture = std::make_shared<TextureAsset>();
	if (!myRHI->LoadTextureFromMemory(myDefaultMaterialTexture.get(),
		L"Default MaterialTexture",
		BuiltIn_Default_M_ByteCode,
		sizeof(BuiltIn_Default_M_ByteCode)))
	{
		return false;
	}
	myDefaultFXTexture = std::make_shared<TextureAsset>();
	if (!myRHI->LoadTextureFromMemory(myDefaultFXTexture.get(),
		L"Default FXTexture",
		BuiltIn_Default_FX_ByteCode,
		sizeof(BuiltIn_Default_FX_ByteCode)))
	{
		return false;
	}

	std::filesystem::path materialPath = "Default Material";
	myDefaultMaterial = std::make_shared<MaterialAsset>(materialPath);
	myDefaultMaterial->SetTexture(L"albedoTexture", myDefaultAlbedoTexture);
	myDefaultMaterial->SetTexture(L"normalTexture", myDefaultNormalTexture);
	myDefaultMaterial->SetTexture(L"materialTexture", myDefaultMaterialTexture);
	myDefaultMaterial->SetTexture(L"fxTexture", myDefaultFXTexture);
	myDefaultMaterials.emplace_back(myDefaultMaterial);
	myCurrentPSO = myShadowPSO;
	return true;
}

void GraphicsEngine::ChangePipelineState(const std::shared_ptr<PipelineStateObject> aNewPSO)
{
	myRHI->ChangePipeLineState(*aNewPSO, *myCurrentPSO);
	myCurrentPSO = aNewPSO;
}

void GraphicsEngine::CreateShaders()
{
	myShaderNames.reserve(30);

#include "CompiledHeaders\DefaultMaterial_VS.h"
#include "CompiledHeaders\DefaultMaterial_PS.h"

		myShaderNames.emplace_back("Default_VS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<VertexShader>());
		std::shared_ptr<VertexShader> vertexShader = std::dynamic_pointer_cast<VertexShader>(myShaders.back());
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *vertexShader, BuiltIn_DefaultMaterial_VS_ByteCode, sizeof(BuiltIn_DefaultMaterial_VS_ByteCode)))
		{
			LOG(GELog, Error, "Failed to load VS-Shader from memory");
		}
		if (!myRHI->CreateInputLayout("Default vertex inputlayout", vertexShader->inputLayout, Vertex::InputLayoutDefinition, BuiltIn_DefaultMaterial_VS_ByteCode, sizeof(BuiltIn_DefaultMaterial_VS_ByteCode)))
		{
			LOG(GELog, Error, "Failed to create default input layout");
		}
		vertexShader->VertexStride = sizeof(Vertex);

		myShaderNames.emplace_back("Default_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		std::shared_ptr<Shader> pixelShader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *pixelShader, BuiltIn_DefaultMaterial_PS_ByteCode, sizeof(BuiltIn_DefaultMaterial_PS_ByteCode)))
		{
			LOG(GELog, Error, "Failed to load PS-Shader from memory");
		}

#include "CompiledHeaders\DirLight_PS.h"

		myShaderNames.emplace_back("Dirlight_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		std::shared_ptr<Shader> dirLightPixelShader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *dirLightPixelShader, BuiltIn_DirLight_PS_ByteCode, sizeof(BuiltIn_DirLight_PS_ByteCode)))
		{
			LOG(GELog, Error, "Failed to load dirlightPS-Shader from memory, {}", myShaderNames.back());
		}

#include "CompiledHeaders\PointLight_PS.h"

		myShaderNames.emplace_back("Pointlight_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		std::shared_ptr<Shader> pointLightPixelShader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *pointLightPixelShader, BuiltIn_PointLight_PS_ByteCode, sizeof(BuiltIn_PointLight_PS_ByteCode)))
		{
			LOG(GELog, Error, "Failed to load pointlightPS-Shader from memory, {}", myShaderNames.back());
		}

#include "CompiledHeaders\SpotLight_PS.h"

		myShaderNames.emplace_back("Spotlight_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		std::shared_ptr<Shader> spotlightPixelShader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *spotlightPixelShader, BuiltIn_SpotLight_PS_ByteCode, sizeof(BuiltIn_SpotLight_PS_ByteCode)))
		{
			LOG(GELog, Error, "Failed to load spotlightPS-Shader from memory, {}", myShaderNames.back());
		}

#include "CompiledHeaders\GBuffer_PS.h"

		myShaderNames.emplace_back("GBuffer_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		std::shared_ptr<Shader> gbufferPixelShader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *gbufferPixelShader, BuiltIn_GBuffer_PS_ByteCode, sizeof(BuiltIn_GBuffer_PS_ByteCode)))
		{
			LOG(GELog, Error, "Failed to load GBufferPS-Shader from memory, {}", myShaderNames.back());
		}

#include "CompiledHeaders/BRDF_PS.h"

		myShaderNames.emplace_back("BRDF_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		std::shared_ptr<Shader> brdfPixelShader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *brdfPixelShader, BuiltIn_BRDF_PS_ByteCode, sizeof(BuiltIn_BRDF_PS_ByteCode)))
		{
			LOG(GELog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}



#include "CompiledHeaders/Quad_VS.h"

		myShaderNames.emplace_back("Quad_VS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<VertexShader>());
		std::shared_ptr<VertexShader> quadVertexShader = std::dynamic_pointer_cast<VertexShader>(myShaders.back());
		if (quadVertexShader == nullptr)
		{
			LOG(GELog, Error, "{} is not a vertex shader", myShaderNames.back());
		}
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *quadVertexShader, BuiltIn_Quad_VS_ByteCode, sizeof(BuiltIn_Quad_VS_ByteCode)))
		{
			LOG(GELog, Error, "Failed to load VS-Shader from memory, {}", myShaderNames.back());
		}
		if (!myRHI->CreateInputLayout("Quad inputlayout", quadVertexShader->inputLayout, Vertex::InputLayoutDefinition, BuiltIn_Quad_VS_ByteCode, sizeof(BuiltIn_Quad_VS_ByteCode)))
		{
			LOG(GELog, Error, "Failed to create input layout for {}", myShaderNames.back());
		}
		quadVertexShader->VertexStride = sizeof(Vertex);


}

void GraphicsEngine::CreateUIShaders()
{
#include "CompiledHeaders/UI_VS.h"
#include "CompiledHeaders/UI_PS.h"

	myShaderNames.emplace_back("UI_VS");
	myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<VertexShader>());
	std::shared_ptr<VertexShader> uiVertexShader = std::dynamic_pointer_cast<VertexShader>(myShaders.back());
	if (uiVertexShader == nullptr)
	{
		LOG(GELog, Error, "{} is not a vertex shader", myShaderNames.back());
	}
	if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *uiVertexShader, BuiltIn_UI_VS_ByteCode, sizeof(BuiltIn_UI_VS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to load VS-Shader from memory, {}", myShaderNames.back());
	}
	if (!myRHI->CreateInputLayout("UI Input layout", uiVertexShader->inputLayout, SpriteVertex::InputLayoutDefinition, BuiltIn_UI_VS_ByteCode, sizeof(BuiltIn_UI_VS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to create input layout for {}", myShaderNames.back());
	}
	uiVertexShader->VertexStride = sizeof(SpriteVertex);
	uiVertexShader->type = ShaderType::VertexShader;

	myShaderNames.emplace_back("UI_PS");
	myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<Shader>());
	std::shared_ptr<Shader> uiPixelShader = myShaders.back();
	if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *uiPixelShader, BuiltIn_UI_PS_ByteCode, sizeof(BuiltIn_UI_PS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
	}
	uiPixelShader->type = ShaderType::PixelShader;

#include "CompiledHeaders/DebugLine_PS.h"
	std::string_view debugLinePS = "DebugLine_PS";
	myShaderMap.emplace(debugLinePS, static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<Shader>());
	myShaders.back()->type = ShaderType::PixelShader;
	std::shared_ptr<Shader> debugLinePixelShader = std::dynamic_pointer_cast<Shader>(myShaders.back());
	if (!myRHI->LoadShaderFromMemory(debugLinePS, *debugLinePixelShader, BuiltIn_DebugLine_PS_ByteCode, sizeof(BuiltIn_DebugLine_PS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to load pixel shader from memory, {}", debugLinePS);
	}
}

void GraphicsEngine::CreateAudioShaders()
{
#include "CompiledHeaders/AudioWave_PS.h"
	std::string_view audioWavePS = "AudioWave_PS";
	myShaderMap.emplace(audioWavePS, static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<Shader>());
	myShaders.back()->type = ShaderType::PixelShader;
	std::shared_ptr<Shader> audioWavePixelShader = std::dynamic_pointer_cast<Shader>(myShaders.back());
	if (!myRHI->LoadShaderFromMemory(audioWavePS, *audioWavePixelShader, BuiltIn_AudioWave_PS_ByteCode, sizeof(BuiltIn_AudioWave_PS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to load pixel shader from memory, {}", audioWavePS);
	}

	{
#include "CompiledHeaders/AudioFade_PS.h"
		std::string_view audioFadePS = "AudioFade_PS";
		myShaderMap.emplace(audioFadePS, static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->type = ShaderType::PixelShader;
		std::shared_ptr<Shader> audiofadePixelShader = std::dynamic_pointer_cast<Shader>(myShaders.back());
		if (!myRHI->LoadShaderFromMemory(audioFadePS, *audiofadePixelShader, BuiltIn_AudioFade_PS_ByteCode, sizeof(BuiltIn_AudioFade_PS_ByteCode)))
		{
			LOG(GELog, Error, "Failed to load AudioFade pixel shader from memory, {}", audioFadePS);
		}
	}
}

void GraphicsEngine::CreateParticleShaders()
{
#include "CompiledHeaders/Particle_VS.h"
	std::string_view particleVSName = "Particle_VS";
	myShaderMap.emplace(particleVSName, static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<VertexShader>());
	std::shared_ptr<VertexShader> particleVertexShader = std::dynamic_pointer_cast<VertexShader>(myShaders.back());
	if (!myRHI->LoadShaderFromMemory(particleVSName, *particleVertexShader, BuiltIn_Particle_VS_ByteCode, sizeof(BuiltIn_Particle_VS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to load vertex shader from memory, {}", particleVSName);
	}
	if (!myRHI->CreateInputLayout("Particle vertex inputlayout", particleVertexShader->inputLayout, ParticleVertex::InputLayoutDefinition, BuiltIn_Particle_VS_ByteCode, sizeof(BuiltIn_Particle_VS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to create input layout for {}", myShaderNames.back());
	}
	particleVertexShader->VertexStride = sizeof(ParticleVertex);


#include "CompiledHeaders/Particle_GS.h"
	std::string_view particleGSName = "Particle_GS";
	myShaderMap.emplace(particleGSName, static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<GeometryShader>());
	std::shared_ptr<GeometryShader> particleGeometryShader = std::dynamic_pointer_cast<GeometryShader>(myShaders.back());
	if (!myRHI->LoadShaderFromMemory(particleGSName, *particleGeometryShader, BuiltIn_Particle_GS_ByteCode, sizeof(BuiltIn_Particle_GS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to load geometry shader from memory, {}", particleGSName);
	}

#include "CompiledHeaders/Particle_PS.h"
	std::string_view particlePSName = "Particle_PS";
	myShaderMap.emplace(particlePSName, static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<Shader>());
	std::shared_ptr<Shader> particlePixelShader = std::dynamic_pointer_cast<Shader>(myShaders.back());
	if (!myRHI->LoadShaderFromMemory(particlePSName, *particlePixelShader, BuiltIn_Particle_PS_ByteCode, sizeof(BuiltIn_Particle_PS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to load pixel shader from memory, {}", particlePSName);
	}

}

void GraphicsEngine::CreatePostProcessShaders()
{

#include "CompiledHeaders/Tonemap_PS.h"

	std::string_view tonemapName = "Tonemap_PS";
	myShaderMap.emplace(tonemapName, static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<Shader>());
	std::shared_ptr<Shader> tonemapPixelShader = myShaders.back();
	if (!myRHI->LoadShaderFromMemory(tonemapName, *tonemapPixelShader, BuiltIn_Tonemap_PS_ByteCode, sizeof(BuiltIn_Tonemap_PS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to load tonemapping PS-Shader from memory, {}", tonemapName);
	}

#include "CompiledHeaders/Luminance_PS.h"

	myShaderNames.emplace_back("Luminance_PS");
	myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<Shader>());
	std::shared_ptr<Shader> luminancePixelShader = myShaders.back();
	if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *luminancePixelShader, BuiltIn_Luminance_PS_ByteCode, sizeof(BuiltIn_Luminance_PS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to load luminance PS-Shader from memory, {}", myShaderNames.back());
	}

#include "CompiledHeaders/GaussianH_PS.h"

	myShaderNames.emplace_back("GaussianH_PS");
	myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<Shader>());
	std::shared_ptr<Shader> gaussianHPixelShader = myShaders.back();
	if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *gaussianHPixelShader, BuiltIn_GaussianH_PS_ByteCode, sizeof(BuiltIn_GaussianH_PS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to load gaussianH PS-Shader from memory, {}", myShaderNames.back());
	}

#include "CompiledHeaders/GaussianV_PS.h"

	myShaderNames.emplace_back("GaussianV_PS");
	myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<Shader>());
	std::shared_ptr<Shader> gaussianVPixelShader = myShaders.back();
	if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *gaussianVPixelShader, BuiltIn_GaussianV_PS_ByteCode, sizeof(BuiltIn_GaussianV_PS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to load gaussianV PS-Shader from memory, {}", myShaderNames.back());
	}

#include "CompiledHeaders/Bloom_PS.h"

	myShaderNames.emplace_back("Bloom_PS");
	myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<Shader>());
	std::shared_ptr<Shader> bloomPixelShader = myShaders.back();
	if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *bloomPixelShader, BuiltIn_Bloom_PS_ByteCode, sizeof(BuiltIn_Bloom_PS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to load bloom PS-Shader from memory, {}", myShaderNames.back());
	}

#include "CompiledHeaders/Resample_PS.h"

	myShaderNames.emplace_back("Resample_PS");
	myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<Shader>());
	std::shared_ptr<Shader> resamplePixelShader = myShaders.back();
	if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *resamplePixelShader, BuiltIn_Resample_PS_ByteCode, sizeof(BuiltIn_Resample_PS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to load Resample-Shader from memory, {}", myShaderNames.back());
	}

#include "CompiledHeaders/SSAO_PS.h"

	myShaderNames.emplace_back("SSAO_PS");
	myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<Shader>());
	std::shared_ptr<Shader> ssaoPixelShader = myShaders.back();
	if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *ssaoPixelShader, BuiltIn_SSAO_PS_ByteCode, sizeof(BuiltIn_SSAO_PS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to load SSAO _PS from memory, {}", myShaderNames.back());
	}

#include "CompiledHeaders/SSAO_Radial_Blur_PS.h"

	myShaderNames.emplace_back("SSAO_Radial_Blur_PS");
	myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<Shader>());
	std::shared_ptr<Shader> ssaoRadialBlurPixelShader = myShaders.back();
	if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *ssaoRadialBlurPixelShader, BuiltIn_SSAO_Radial_Blur_PS_ByteCode, sizeof(BuiltIn_SSAO_Radial_Blur_PS_ByteCode)))
	{
		LOG(GELog, Error, "Failed to load SSAO_Radial_Blur_PS from memory");
	}
}

void GraphicsEngine::InitPostProcessBuffer()
{
	std::uniform_real_distribution<float> randomValues(0, 1);
	std::default_random_engine randomEngine;
	PostProcessBuffer ppbuffer;
	for (int index = 0; index < kernelSize; index++)
	{
		CU::Vector4f v =
		{
			randomValues(randomEngine) * 2.0f - 1.0f,
			randomValues(randomEngine) * 2.0f - 1.0f,
			randomValues(randomEngine),
			0
		};
		v = v.GetNormalized();
		float s = static_cast<float>(index) / static_cast<float>(kernelSize);
		s = CU::Lerp(0.1f, 1.0f, s * s);
		v = v * s;
		ppbuffer.PostProcessData.Kernel[index] = v;
	}
	if (!UpdateAndSetConstantBuffer(ConstantBufferType::PostProcessBuffer, ppbuffer))
	{
		LOG(GELog, Error, "Failed setting post process buffer data!");
	}
}


void GraphicsEngine::EndFrame()
{
	myRHI->Present(0);
}

#pragma once
#include "Objects/MeshAsset.h"
#include "Commands/GraphicsCommandList.h"
#include "Buffers/ConstantBuffer.h"
#include "Shader.h"
#include "..\GameEngine\Components\CameraComponent.h"
#include "..\GameEngine\Components\MeshComponent.h"
#include "Commands/CmdEnums.h"
#include "InterOp/PipelineStateObject.h"
#include "Buffers\GBuffer.h"
#include "ViewPort.h"

#include "InterOp\RHI.h"
#include "InterOp\RHIStructs.h"

#include <wrl.h>
#include <memory>
#include <vector>
using namespace Microsoft::WRL;

struct PostProcessBuffer;
class WindowHandler;
class RHI;
class TextureAsset;
class MaterialAsset;
class SpriteAsset;
class DebugLineObject;
struct LightSources;
struct DirectionalLightData;
struct PointLightData;
struct SpotLightData;
class ParticleEmitter;
class InstanceData;

enum class ConstantBufferType : unsigned
{
	ObjectBuffer,
	FrameBuffer,
	AnimationBuffer,
	MaterialBuffer,
	LightBuffer,
	DebugBuffer,
	PostProcessBuffer
};

enum class ShadowMaps
{
	DirLightShadowMap,
	PointLightShadowMap,
	SpotLightShadowMap
};


class GraphicsEngine
{
private:
	// We're a container singleton, no instancing this outside the class.
	GraphicsEngine() = default;

public:

	static GraphicsEngine& Get();
	//static GraphicsEngine& Get() { static GraphicsEngine myInstance; return myInstance; }

	static const std::unique_ptr<RHI>& GetRHI();

	CU::Vector2<unsigned> GetDeviceSize() const;

	CU::Vector2f GetViewPortSize();

	ViewPort GetViewPort();

	bool InitWindow(SIZE aWindowSize, WNDPROC aWindowProcess, LPCWSTR aWindowTitle);

	void ShowSplashScreen();
	void HideSplashScreen();

	bool Initialize(bool enableDeviceDebug);

	void Update();

	template<typename BufferData>
	bool UpdateAndSetConstantBuffer(ConstantBufferType aBufferType, const BufferData& aDataBlock);

	template<typename VertexType>
	bool UpdateVertexBuffer(const VertexBuffer& aVertexBuffer, const std::vector<VertexType>& aVertexList);

	bool PrepareParticleEmitter(ParticleEmitter& anEmitter);


	bool ClearTextureResourceSlot(unsigned aPipelineStages, unsigned aSlot) const;


	bool LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, const size_t& aTextureDataSize, TextureAsset& outTexture) const;
	bool LoadTexture(const std::filesystem::path& aFilePath, TextureAsset& outTexture) const;

	bool CreateTexture(const std::filesystem::path& aName, unsigned aWidth, unsigned aHeight, RHITextureFormat aFormat,
		TextureAsset* outTexture, bool aStaging, bool aShaderResource, bool aRenderTarget, bool aCpuAccessRead, bool aCpuAccessWrite) const;

	bool CreateSRV(TextureAsset* outTexture, std::string_view aName) const;

	std::shared_ptr<MaterialAsset> GetDefaultMaterial() const;

	void BeginEvent(std::string_view aEvent) const;
	void EndEvent() const;
	void SetMarker(std::string_view aMarker) const;


	void DrawQuad(const CU::Vector2f& aSize = {-1.0f, -1.0f});


	void EndFrame();


	void RenderMesh(const MeshAsset& aMesh, const std::vector<std::shared_ptr<MaterialAsset>>& someMaterials) const;

	void RenderSprite2D(const SpriteAsset& anElement) const;

	void RenderAudioUI(unsigned aStartIndex, unsigned anIndexCount);

	void RenderInstancedMesh(const MeshAsset& aMesh, const std::vector<std::shared_ptr<MaterialAsset>>& someMaterials, const InstanceData& anInstanceData) const;

	void RenderPoints(const unsigned aParticleCount);

	void RenderDebugLines(const DebugLineObject& aDebugLineObject);

	void ConfigureInputAssembler(unsigned aTopology, const ComPtr<ID3D11Buffer>& aVxBuffer, const ComPtr<ID3D11Buffer>& anIxBuffer, unsigned aVertexStride, const ComPtr<ID3D11InputLayout>& anInputLayout);

	bool SetTextureResource(unsigned aPipelineStages, unsigned aSlot, const TextureAsset& aTexture) const;

	void SetPixelShader(const std::string_view& aShaderName);
	void SetVertexShader(const std::string_view& aShaderName);
	void SetGeometryShader(const std::string_view& aShaderName);

	void SetShadowMap(ShadowMaps aShadowMap, unsigned anIndex = 0);
	void SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> aVertexBuffer);
	void SetIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> anIndexBuffer);
	void ChangePipelineState(const unsigned aNewPipelineState);
	void ChangeRenderTarget(const std::shared_ptr<TextureAsset>& aRenderTarget) const;

	void ClearBackBuffer() const;
	void ClearRenderTarget();
	void ClearRenderTarget(const std::shared_ptr<TextureAsset>& aRenderTarget) const;

	template<typename VertexType>
	void CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& aOutVxBuffer, bool aDynamic = false);

	void CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& aOutIxBuffer);

	const std::vector<std::shared_ptr<MaterialAsset>>& GetDefaultMaterials()const;

	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetForwardPSO() const { return myForwardPSO; }
	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetShadowPSO() const { return myShadowPSO; }
	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetPointShadowPSO() const { return myPointShadowPSO; }
	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetSpotShadowPSO() const { return mySpotShadowPSO; }
	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetDeferredPSO() const { return myDeferredPSO; }
	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetDirLightPSO() const { return myDirLightPSO; }
	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetPointLightPSO() const { return myPointLightPSO; }
	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetSpotLightPSO() const { return mySpotLightPSO; }
	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetParticlePSO() const { return myParticlePSO; }
	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetTonemapPSO() const { return myTonemapPSO; }
	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetLuminancePSO() const { return myLuminancePSO; }
	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetResamplingPSO() const { return myResamplingPSO; }
	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetGaussianHPSO() const { return myGaussianHPSO; }
	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetGaussianVPSO() const { return myGaussianVPSO; }
	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetBloomPSO() const { return myBloomPSO; }
	FORCEINLINE const std::shared_ptr<PipelineStateObject> GetCustomPSO() const { return myCustomPSO; }

	FORCEINLINE const std::shared_ptr<TextureAsset> GetShadowMap() const { return myDirLightShadowMap; }
	std::shared_ptr<TextureAsset> GetLuminanceBuffer() { return myLuminanceBuffer; }
	std::shared_ptr<TextureAsset> GetLDRBuffer() { return myLDRBuffer; }
	std::shared_ptr<TextureAsset> GetHDRBuffer() { return myHDRBuffer; }
	std::shared_ptr<TextureAsset> GetHalfSizeBuffer() { return myHalfScreenBuffer; }
	std::shared_ptr<TextureAsset> GetHalfSizeBufferB() { return myHalfScreenBufferB; }
	std::shared_ptr<TextureAsset> GetQuarterSizeBuffer() { return myQuarterScreenBuffer; }
	std::shared_ptr<TextureAsset> GetEightSizeBufferA() { return myEighthScreenBufferA; }
	std::shared_ptr<TextureAsset> GetEightSizeBufferB() { return myEighthScreenBufferB; }
	std::shared_ptr<TextureAsset> GetSSAOBuffer() { return mySSAOBuffer; }
	std::shared_ptr<TextureAsset> GetBlueNoiseTexture() { return myBluenoiseTexture; }
	std::shared_ptr<TextureAsset> GetBackBuffer() { return myRHI->GetBackBuffer(); }

	HWND GetWindowHandle() const;
	SIZE GetWindowSize() const;
	CU::Vector2f GetWindowSizeAsVector() const;


private:
	bool CreateForwardPSO();
	bool CreateShadowPSO();
	bool CreatePointShadowPSO();
	bool CreateSpotShadowPSO();
	bool CreateDeferredPSO();
	bool CreateDirLightPSO();
	bool CreatePointLightPSO();
	bool CreateSpotLightPSO();
	bool CreateSpritePSO();
	bool CreateTonemapPSO();
	bool CreateResamplingPSO();
	bool CreateLuminancePSO();
	bool CreateGaussianHPSO();
	bool CreateGaussianVPSO();
	bool CreateBloomPSO();
	bool CreateSSAOPSO();
	bool CreateParticlePSO();

	bool CreateDirLightShadowMap(const std::string& aName);
	void CreatePointLightShadowMaps(const std::string& aName);
	void CreateSpotLightShadowMaps(const std::string& aName);
	bool CreateLUTTexture();
	bool CreateGBuffer();
	bool ClearGBuffer() const;
	bool SetGBufferResource() const;
	bool CreateAdditiveBlendState();
	void CreateIntermediateBuffers();

	bool CreateDefaultMaterials();

	void ChangePipelineState(const std::shared_ptr<PipelineStateObject> aNewPSO);

	void CreateShaders();

	void CreateUIShaders();

	void CreateAudioShaders();

	void CreateParticleShaders();

	void CreatePostProcessShaders();

	void InitPostProcessBuffer();

private:
	friend class GraphicsCommand;
	friend struct GraphicsCommandBase;

	std::unique_ptr<WindowHandler> myWindowHandler;
	//SIZE myWindowSize{ 0,0 };
	//HWND myWindowHandle{};

	std::unique_ptr<RHI> myRHI;
	std::shared_ptr<PipelineStateObject> myCurrentPSO;
	std::shared_ptr<PipelineStateObject> myForwardPSO;
	std::shared_ptr<PipelineStateObject> myShadowPSO;
	std::shared_ptr<PipelineStateObject> myPointShadowPSO;
	std::shared_ptr<PipelineStateObject> mySpotShadowPSO;
	std::shared_ptr<PipelineStateObject> myDeferredPSO;
	std::shared_ptr<PipelineStateObject> myDirLightPSO;
	std::shared_ptr<PipelineStateObject> myPointLightPSO;
	std::shared_ptr<PipelineStateObject> mySpotLightPSO;
	std::shared_ptr<PipelineStateObject> mySpritePSO;
	std::shared_ptr<PipelineStateObject> myParticlePSO;
	std::shared_ptr<PipelineStateObject> myTonemapPSO;
	std::shared_ptr<PipelineStateObject> myResamplingPSO;
	std::shared_ptr<PipelineStateObject> myLuminancePSO;
	std::shared_ptr<PipelineStateObject> myGaussianHPSO;
	std::shared_ptr<PipelineStateObject> myGaussianVPSO;
	std::shared_ptr<PipelineStateObject> myBloomPSO;
	std::shared_ptr<PipelineStateObject> mySSAOPSO;
	std::shared_ptr<PipelineStateObject> myCustomPSO;

	GBufferData myGBuffer;

	std::shared_ptr<TextureAsset> myDefaultAlbedoTexture;
	std::shared_ptr<TextureAsset> myDefaultNormalTexture;
	std::shared_ptr<TextureAsset> myDefaultMaterialTexture;
	std::shared_ptr<TextureAsset> myDefaultFXTexture;
	std::shared_ptr<MaterialAsset> myDefaultMaterial;
	std::vector<std::shared_ptr<MaterialAsset>> myDefaultMaterials;

	std::shared_ptr<TextureAsset> myLUTTexture;
	std::shared_ptr<TextureAsset> myLuminanceBuffer;;
	std::shared_ptr<TextureAsset> myHalfScreenBuffer;
	std::shared_ptr<TextureAsset> myHalfScreenBufferB;
	std::shared_ptr<TextureAsset> myQuarterScreenBuffer;
	std::shared_ptr<TextureAsset> myEighthScreenBufferA;
	std::shared_ptr<TextureAsset> myEighthScreenBufferB;
	std::shared_ptr<TextureAsset> myLDRBuffer;
	std::shared_ptr<TextureAsset> mySSAOBuffer;
	std::shared_ptr<TextureAsset> myHDRBuffer;

	std::shared_ptr<TextureAsset> myBluenoiseTexture;

	std::shared_ptr<TextureAsset> myDirLightShadowMap;
	std::array<std::shared_ptr<TextureAsset>, 4> myPointLightShadowMaps;
	std::array<std::shared_ptr<TextureAsset>, 4> mySpotLightShadowMaps;
	ComPtr<ID3D11SamplerState> myShadowSampler;
	ComPtr<ID3D11VertexShader> myShadowShader;

	ComPtr<ID3D11SamplerState> myDefaultSampler;

	std::unordered_map<ConstantBufferType, ConstantBuffer> myConstantBuffers;

	std::unordered_map<std::string_view, unsigned> myShaderMap;
	std::vector<std::string> myShaderNames;
	std::vector<std::shared_ptr<Shader>> myShaders;
	std::shared_ptr<Shader> myCurrentPixelShader;
	std::shared_ptr<VertexShader> myCurrentVertexShader;
	std::shared_ptr<GeometryShader> myCurrentGeometryShader;
};


template<typename BufferData>
inline bool GraphicsEngine::UpdateAndSetConstantBuffer(ConstantBufferType aBufferType, const BufferData& aDataBlock)
{
	if (!myConstantBuffers.contains(aBufferType))
	{
		return false;
	}

	ConstantBuffer& buffer = myConstantBuffers.at(aBufferType);
	if (!myRHI->UpdateConstantBuffer(buffer, aDataBlock))
	{
		return false;
	}

	myRHI->SetConstantBuffer(buffer);
	return true;
}

template<typename VertexType>
inline bool GraphicsEngine::UpdateVertexBuffer(const VertexBuffer& aVertexBuffer, const std::vector<VertexType>& aVertexList)
{
	return myRHI->UpdateVertexBuffer(aVertexBuffer, aVertexList);
}

template<typename VertexType>
inline void GraphicsEngine::CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& aOutVxBuffer, bool aDynamic)
{
	myRHI->CreateVertexBuffer(aName, aVertexList, aOutVxBuffer, aDynamic);
}



#pragma once

#include <array>
#include "d3d11.h"
#include "d3d11_1.h"
#include "d3d11shader.h"
#include "d3dcompiler.h"

#include <memory>
#include <string>
#include <vector>

#include "../Utilities/CommonUtilities/Vector2.hpp"
#include "ShaderInfo.h"
#include "RHIStructs.h"
#include "ViewPort.h"
#include "../Buffers/VertexBuffer.h"
#include "../Buffers/DebugBuffer.h"

#include <filesystem>

#pragma comment(lib, "d3dcompiler.lib")

#include <unordered_map>
#include <wrl.h>
struct Shader;
using namespace Microsoft::WRL;

enum class RHITextureFormat : unsigned
{
	R32G32B32A32_Float = 2,
	R16G16B16A16_FLOAT = 10,
	R16G16B16A16_SNORM = 13,
	R32G8X24_Typeless = 19,
	D32_Float_S8X24_UINT = 20,
	R32_Float_X8X24_Typeless = 21,
	R8G8B8A8_UNORM = 28,
	R16G16_Float = 34,
	R32_Typeless = 39,
	D32_Float = 40,
	R32_Float = 41,
	R32_UINT = 42
};

class TextureAsset;
class ConstantBuffer;
struct VertexBuffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3DUserDefinedAnnotation;
struct ID3D11ShaderReflection;
struct PipelineStateObject;



enum RasterizerState
{
	RS_Default,
	RS_CullNone,
	RS_Wireframe,
	RS_COUNT
};

enum DepthState
{
	DS_Default,
	DS_LessEqual,
	DS_ReadOnly,
	DS_LessEqualWrite,
	DS_COUNT
};


class RHI
{
public:

	struct DeviceSize
	{
		unsigned Width;
		unsigned Height;
	};

	bool Initialize(HWND aWindowHandle, bool enableDeviceDebug);

	bool UnInitialize();

	void Present(unsigned aSyncInterval = 0);

	const DeviceSize& GetDeviceSize() { return myDeviceSize; }

	const Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetSamplerState(const std::string& aName);

	template<typename VertexType>
	bool CreateVertexBuffer(const std::string_view& aName, const std::vector<VertexType>& aVertexList, ComPtr<ID3D11Buffer>& outVxBuffer, bool aIsDynamic = false);

	bool CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer, bool aIsDynamic = false);

	bool CreateConstantBuffer(std::string_view aName, size_t aSize, unsigned aSlot, unsigned aPipelineStages, ConstantBuffer& outBuffer);

	template<typename BufferData>
	bool UpdateConstantBuffer(const ConstantBuffer& aBuffer, const BufferData& aBufferData);

	template<typename VertexType>
	bool UpdateVertexBuffer(const VertexBuffer& aVertexBuffer, const std::vector<VertexType>& aVertexList);

	bool UpdateIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& aIndexBuffer, const std::vector<unsigned>& aIndexList);

	bool CreateSamplerState(std::string_view aName,  const D3D11_SAMPLER_DESC& aDescription);

	bool CreateInputLayout(std::string_view aName, Microsoft::WRL::ComPtr<ID3D11InputLayout>& outInputLayout, const std::vector<VertexElementDesc>& aInputLayoutDefinition, const uint8_t* aShaderDataPtr, size_t aShaderDataSize);

	bool LoadTexture(TextureAsset* outTexture, const std::filesystem::path& aFileName);

	bool LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize, TextureAsset& outTexture) const;


	bool LoadShaderFromMemory(std::string_view aName, Shader& outShader, const uint8_t* aShaderDataPtr, size_t aShaderDataSize);

	bool LoadTextureFromMemory(TextureAsset* outTexture, const std::filesystem::path& aName, const BYTE* someImageData, size_t anImageDataSize, const
		D3D11_SHADER_RESOURCE_VIEW_DESC* aSRVDesc = nullptr);


	void ConfigureInputAssembler(unsigned aTopology, const ComPtr<ID3D11Buffer>& aVxBuffer, const ComPtr<ID3D11Buffer>& anIxBuffer, unsigned aVertexStride, const ComPtr<ID3D11InputLayout>& anInputLayout);


	void SetVertexShader(const ComPtr<ID3D11VertexShader>& aVertexShader);
	void SetVertexShader(std::shared_ptr<Shader> aPixelShader);
	void SetPixelShader(std::shared_ptr<Shader> aPixelShader);
	void SetGeometryShader(std::shared_ptr<Shader> aGeometryShader);
	void SetComputeShader(std::shared_ptr<Shader> aComputeShader);
	void SetCSShaderResource(unsigned aSlot, const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aSRV);
	void SetCSUnorderedAccessView(unsigned aSlot, const Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& aUAV);

	void SetRenderTarget(std::shared_ptr<TextureAsset> aRenderTarget) const;

	void SetRasterizerState(const RasterizerState aState);

	void SetDepthState(const DepthState aState);

	void SetObjectName(ComPtr<ID3D11DeviceChild> anObject, std::string_view aName) const;

	void DrawIndexed(unsigned aStartIndex, unsigned aIndexCount) const;

	void DrawIndexedInstanced(unsigned anIndexCount, unsigned anInstanceCount);

	void Draw(unsigned aCount);

	void Dispatch(unsigned aThreadCountX, unsigned aThreadCountY, unsigned aThreadCountZ);

	void ClearRenderTargets(const int& aNumViews = 1) const;
	void ClearBackBuffer() const;
	void ClearViewportBackBuffer() const;

	bool CreateTexture(std::string aName, unsigned aWidth, unsigned aHeight, RHITextureFormat aFormat,
		TextureAsset* outTexture, bool aStaging, bool aShaderResource, bool aRenderTarget, bool aCpuAccessRead, bool aCpuAccessWrite) const;

	bool CreateSRV(TextureAsset* outTexture, std::string_view aName);

	bool CreateStagingBuffer(size_t aByteWidth, Microsoft::WRL::ComPtr<ID3D11Buffer>& outStagingBuffer);
	bool CreateStagingTexture2D(unsigned aWidth, unsigned aHeight, DXGI_FORMAT aFormat, Microsoft::WRL::ComPtr<ID3D11Texture2D>& outStagingTexture);
	bool CreateStructuredBuffer(size_t aStride, size_t aElementCount, const void* aInitData, Microsoft::WRL::ComPtr<ID3D11Buffer>& outBuffer, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& outSRV);
	bool CreateUAVBuffer(size_t aStride, size_t aElementCount, Microsoft::WRL::ComPtr<ID3D11Buffer>& outBuffer, Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& outUAV);


	void ClearRenderTarget(const TextureAsset* aTexture, std::array<float, 4> aClearColor = { 0, 0, 0, 0 });

	void ClearDepthStencil(const TextureAsset* aTexture);


	bool SetTextureResource(unsigned aPipelineStages, unsigned aSlot, const TextureAsset& aTexture) const;
	bool ClearTextureResourceSlot(unsigned aPipelineStages, unsigned aSlot) const;

	void BeginEvent(std::string_view anEventName);
	void EndEvent();
	void SetMarker(std::string_view aMarker);

	void DrawQuad(const CU::Vector2f& aSize);

	Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() { return myDevice; }
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() { return myContext; }

	void ChangePipeLineState(const PipelineStateObject& aNewPSO, const PipelineStateObject& anOldPSO);

	void ChangeDepthStencil(const std::shared_ptr<TextureAsset> aDepthTexture);

	bool CreateShadowMap(const std::string& aName, CommonUtilities::Vector2<unsigned> aSize,
		TextureAsset& aOutTexture);

	bool CreateLUTTexture(TextureAsset& aTexture, const std::shared_ptr<Shader>& aBrdfPixelShader, const std::shared_ptr<Shader>& aQuadVertexShader);

	bool CreateBlendState(std::string_view aName, const D3D11_BLEND_DESC& aBlendDesc);
	const Microsoft::WRL::ComPtr<ID3D11BlendState>& GetBlendState(const std::string& aName) const;

	const Microsoft::WRL::ComPtr<ID3D11DepthStencilState>& GetDepthStencilState(DepthState aDepthState) const;

	void SetVertexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, size_t aVertexSize, size_t aVertexOffset);
	void SetVertexBuffers(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, const Microsoft::WRL::ComPtr<ID3D11Buffer>& anInstanceBuffer, size_t aVertexSize, size_t anInstanceSize);
	void SetIndexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aIndexBuffer);
	void SetConstantBuffer(const ConstantBuffer& aBuffer);
	void SetPrimitiveTopology(unsigned aTopology);
	void SetInputLayout(const Microsoft::WRL::ComPtr<ID3D11InputLayout>& aInputLayout);

	template<typename T>
	T ReadTexture2DPixel(ID3D11Resource* aSourceTexture, ID3D11Resource* aStagingTexture, unsigned aX, unsigned aY);

	template<typename T>
	std::vector<T> ReadStructuredBuffer(ID3D11Buffer* aSourceBuffer, ID3D11Buffer* aStagingBuffer, size_t aElementCount);

	template<typename T>
	bool UpdateDynamicBuffer(ID3D11Buffer* aBuffer, const std::vector<T>& aData);

	void ResizeViewport(const unsigned aWidth, const unsigned aHeight);

	void ReleaseStaleObjects();

	FORCEINLINE std::shared_ptr<TextureAsset> GetBackBuffer() const { return myBackBuffer; }
	FORCEINLINE std::shared_ptr<TextureAsset> GetDepthBuffer() const { return myDepthBuffer; }
	FORCEINLINE std::shared_ptr<TextureAsset> GetViewportBackBuffer() const { return myViewportBackBuffer; }

	CU::Vector2f GetViewPortSize() const;

	ViewPort GetViewPort();

private:
	bool CreateVertexBufferInternal(const std::string_view& aName, ComPtr<ID3D11Buffer>& outVxBuffer, const uint8_t* aVertexDataPtr, size_t aNumVertices, size_t aVertexSize, bool aIsDynamic);
	bool UpdateConstantBufferInternal(const ConstantBuffer& aBuffer, const void* aBufferData, size_t aBufferDataSize) const;
	bool UpdateVertexBufferInternal(const VertexBuffer& aVertexBuffer, const uint8_t* aBufferData, size_t aNumVertices,  size_t aVertexSize) const;

private:
	ComPtr<ID3D11Device> myDevice;
	ComPtr<ID3D11DeviceContext> myContext;
	std::shared_ptr<TextureAsset> myBackBuffer;
	std::shared_ptr<TextureAsset> myDepthBuffer;
	std::shared_ptr<TextureAsset> myViewportBackBuffer;
	std::shared_ptr<TextureAsset> myViewportDepthBuffer;
	ComPtr<IDXGISwapChain> mySwapChain;
	ComPtr<ID3DUserDefinedAnnotation> myAnnotationObject;
	DeviceSize myDeviceSize;
	std::array<ComPtr<ID3D11RasterizerState>, RS_COUNT> myRasterizerStates;
	std::array<ComPtr<ID3D11DepthStencilState>, DS_COUNT> myDepthStates;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> mySamplerStates;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11BlendState>> myBlendStates;

	ComPtr<ID3D11Resource> myStaleViewportTex;
	ComPtr<ID3D11RenderTargetView> myStaleViewportRTV;
	ComPtr<ID3D11ShaderResourceView> myStaleViewportSRV;

	ComPtr<ID3D11DepthStencilView> myStaleDepthDSV;
};

template <typename VertexType>
bool RHI::CreateVertexBuffer(const std::string_view& aName, const std::vector<VertexType>& aVertexList, ComPtr<ID3D11Buffer>& outVxBuffer, bool aIsDynamic)
{
	const size_t vxSize = sizeof(VertexType);
	const size_t numVx = aVertexList.size();
	return CreateVertexBufferInternal(aName, outVxBuffer, reinterpret_cast<const uint8_t*>(aVertexList.data()), numVx, vxSize, aIsDynamic);
}

template<typename BufferData>
inline bool RHI::UpdateConstantBuffer(const ConstantBuffer& aBuffer, const BufferData& aBufferData)
{
	const size_t dataSize = sizeof(BufferData);
	return UpdateConstantBufferInternal(aBuffer, &aBufferData, dataSize);
}

template<typename VertexType>
bool RHI::UpdateVertexBuffer(const VertexBuffer& aVertexBuffer, const std::vector<VertexType>& aVertexList)
{
	const size_t dataSize = sizeof(VertexType);
	return UpdateVertexBufferInternal(aVertexBuffer, reinterpret_cast<const uint8_t*>(aVertexList.data()), aVertexList.size(), dataSize);
}

template<typename T>
inline T RHI::ReadTexture2DPixel(ID3D11Resource* aSourceTexture, ID3D11Resource* aStagingTexture, unsigned aX, unsigned aY)
{
	if (!aSourceTexture || !aStagingTexture) return T();

	D3D11_BOX srcBox;
	srcBox.left = aX;
	srcBox.right = aX + 1;
	srcBox.top = aY;
	srcBox.bottom = aY + 1;
	srcBox.front = 0;
	srcBox.back = 1;

	myContext->CopySubresourceRegion(aStagingTexture, 0, 0, 0, 0, aSourceTexture, 0, &srcBox);

	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr = myContext->Map(aStagingTexture, 0, D3D11_MAP_READ, 0, &mapped);

	if (SUCCEEDED(hr))
	{
		T result = *(reinterpret_cast<T*>(mapped.pData));
		myContext->Unmap(aStagingTexture, 0);
		return result;
	}

	return T();
}

template<typename T>
inline std::vector<T> RHI::ReadStructuredBuffer(ID3D11Buffer* aSourceBuffer, ID3D11Buffer* aStagingBuffer, size_t aElementCount)
{
	if (!aSourceBuffer || !aStagingBuffer || aElementCount == 0) return {};

	myContext->CopyResource(aStagingBuffer, aSourceBuffer);

	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr = myContext->Map(aStagingBuffer, 0, D3D11_MAP_READ, 0, &mapped);

	if (SUCCEEDED(hr))
	{
		T* rawData = reinterpret_cast<T*>(mapped.pData);
		std::vector<T> results(rawData, rawData + aElementCount);

		myContext->Unmap(aStagingBuffer, 0);
		return results;
	}

	return {};
}

template<typename T>
bool RHI::UpdateDynamicBuffer(ID3D11Buffer* aBuffer, const std::vector<T>& aData)
{
	if (!aBuffer || aData.empty()) return false;

	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr = myContext->Map(aBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

	if (SUCCEEDED(hr))
	{
		memcpy(mapped.pData, aData.data(), aData.size() * sizeof(T));
		myContext->Unmap(aBuffer, 0);
		return true;
	}
	return false;
}



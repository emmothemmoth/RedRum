#include "GraphicsEngine.pch.h"
#include "RHI.h"
#include "comdef.h"
#include <fstream>

#include "DDSTextureLoader11.h"
#include "Helpers.h"
#include "Objects/Vertex.h"
#include "Objects/ParticleVertex.h"
#include "Objects/TextureAsset.h"
#include "Buffers/ConstantBuffer.h"
#include "PipelineStateObject.h"

#include "Logger\Logger.h"
#include "StringHelpers.h"

#include "d3d11shader.h"
#include "d3dcompiler.h"
#include "Shader.h"


#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(RHILog, "RHI", Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(RHILog, "RHI", Error);
#endif

DEFINE_LOG_CATEGORY(RHILog);

template<typename T, typename STRINGTYPE>
inline static T string_cast(const STRINGTYPE& someString) = delete;
template<typename T, typename CHARTYPE>
inline static T string_cast(const CHARTYPE* someString) = delete;

template<>
inline std::string string_cast<std::string>(const std::wstring& someString)
{
	const int sLength = static_cast<int>(someString.length());
	const int len = WideCharToMultiByte(CP_ACP, 0, someString.c_str(), sLength, 0, 0, 0, 0);
	std::string result(len, L'\0');
	WideCharToMultiByte(CP_ACP, 0, someString.c_str(), -1, result.data(), len, 0, 0);
	return result;
}

template<>
inline std::wstring string_cast<std::wstring>(const std::string& someString)
{
	const int sLength = static_cast<int>(someString.length());
	const int len = MultiByteToWideChar(CP_ACP, 0, someString.c_str(), sLength, 0, 0);
	std::wstring result(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, someString.c_str(), -1, result.data(), len);
	return result;
}

template<>
inline std::string string_cast<std::string>(const wchar_t* someString)
{
	const std::wstring str = someString;
	return string_cast<std::string>(str);
}

template<>
inline std::wstring string_cast<std::wstring>(const char* someString)
{
	const std::string str = someString;
	return string_cast<std::wstring>(str);
}

bool RHI::Initialize(HWND aWindowHandle, bool enableDeviceDebug)
{
	myBackBuffer = std::make_shared<TextureAsset>();

	HRESULT result = E_FAIL;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = aWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;

	// More efficient use on Win 8+ but cannot be used on Win 7.
	// Requires Buffercount to be 2.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// If we don't set this DX will lock the refresh date to the monitor
	// refresh rate (even if we don't use V-Sync).
	// It essentially means that we may restart drawing a frame even if
	// doing so would result in a half-drawn frame being put on the screen,
	// i.e. so called screen tearing.
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	swapChainDesc.BufferCount = 2;

	result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		enableDeviceDebug ? D3D11_CREATE_DEVICE_DEBUG : 0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mySwapChain,
		&myDevice,
		nullptr,
		&myContext
	);

	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create Device and Swap Chain!");
		return false;
	}

	// Create the BackBuffer object that our Graphics Engine will use.
	// This is a special case that can't use our own CreateTexture function because
	// DirectX has already created its own representation via D3D11CreateDeviceAndSwapChain
	// and we just need a place to contain it for our own purposes.
	//outBackBuffer = std::make_shared<Texture>();
	myBackBuffer->myName = "RHI Backbuffer";
	myBackBuffer->myBindFlags = D3D11_BIND_RENDER_TARGET;
	myBackBuffer->myUsageFlags = D3D11_USAGE_DEFAULT;
	myBackBuffer->myAccessFlags = 0;

	// Retrieve back buffer texture that was created when we called CreateDeviceAndSwapChain.
	result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &myBackBuffer->myTexture);
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to retrieve backbuffer resource!");
		return false;
	}

	// Create a View for the BackBuffer Texture that allows us to draw on the texture.
	result = RHI::myDevice->CreateRenderTargetView(myBackBuffer->myTexture.Get(),
		nullptr, myBackBuffer->myRTV.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create RTV for backbuffer!");
		return false;
	}

	// Retrieve the display area in the Window. This is the area
	// we can actually do something with, not including the title
	// bar and border.
	RECT clientRect;
	ZeroMemory(&clientRect, sizeof(RECT));
	GetClientRect(aWindowHandle, &clientRect);
	const unsigned int windowWidth = clientRect.right - clientRect.left;
	const unsigned int windowHeight = clientRect.bottom - clientRect.top;

	myDeviceSize = { windowWidth, windowHeight };

	// We also need a viewport that specifies which area of that Texture to draw on.
	myBackBuffer->myViewPort = { 0.0f, 0.0f,
		static_cast<float>(windowWidth), static_cast<float>(windowHeight),
		0.0f, 1.0f };

	// Next we need to describes the scene depth. This will allow proper
	// behavior when our 3D world is projected onto our 2D monitor.
	myDepthBuffer = std::make_shared<TextureAsset>();
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = static_cast<unsigned int>(windowWidth);
	desc.Height = static_cast<unsigned int>(windowHeight);
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> depthTexture;
	// For the Scene Depth we can just use our own function to create a depth texture.
	result = myDevice->CreateTexture2D(&desc, nullptr, depthTexture.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Faield to create depth buffer");
		return false;
	}

	SetObjectName(depthTexture, "DepthBuffer_T2D");

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	result = myDevice->CreateDepthStencilView(depthTexture.Get(), &dsvDesc, myDepthBuffer->myDSV.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create depth stencil view");
		return false;
	}

	SetObjectName(myDepthBuffer->myDSV, "DepthBuffer_DSV");

	// If we got this far we've initialized DirectX! Yay!

	// Now we should create all states we need to keep track of
	myRasterizerStates[RS_Default] = nullptr;

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;
	result = myDevice->CreateRasterizerState(&rasterizerDesc, myRasterizerStates[RS_CullNone].GetAddressOf());
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create default rasterizer state");
		return false;
	}

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	myDevice->CreateRasterizerState(&rasterizerDesc, myRasterizerStates[RS_Wireframe].GetAddressOf());

	myDepthStates[DS_Default] = nullptr;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = false;
	result = myDevice->CreateDepthStencilState(&depthStencilDesc, myDepthStates[DS_LessEqual].GetAddressOf());
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create default depth stencil state");
		return false;
	}

	myDepthStates[DS_ReadOnly] = nullptr;
	D3D11_DEPTH_STENCIL_DESC readOnlyDepth = CD3D11_DEPTH_STENCIL_DESC();
	readOnlyDepth.DepthEnable = true;
	readOnlyDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	readOnlyDepth.DepthFunc = D3D11_COMPARISON_LESS;
	readOnlyDepth.StencilEnable = false;
	result = myDevice->CreateDepthStencilState(&readOnlyDepth, myDepthStates[DS_ReadOnly].GetAddressOf());
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create read-only depth stencil state");
		return false;
	}

	myContext->QueryInterface(IID_PPV_ARGS(&myAnnotationObject));
	SetObjectName(myContext, "Device Context");
	SetObjectName(myBackBuffer->myRTV, "Backbuffer RTV");
	LOG(RHILog, Log, "RHI Initialized.");
	return true;
}

bool RHI::UnInitialize()
{
	myDevice.Reset();
	myContext.Reset();
	mySwapChain.Reset();

	return true;
}

void RHI::Present(unsigned aSyncInterval)
{
	mySwapChain->Present(aSyncInterval, DXGI_PRESENT_ALLOW_TEARING);
}

const Microsoft::WRL::ComPtr<ID3D11SamplerState>& RHI::GetSamplerState(const std::string& aName)
{
	return mySamplerStates.at(aName);
}

CU::Vector2f RHI::GetViewPortSize() const
{
	return myBackBuffer->GetSize();
}

ViewPort RHI::GetViewPort()
{
	return myBackBuffer->myViewPort;
}


bool RHI::CreateVertexBufferInternal(const std::string_view& aName, ComPtr<ID3D11Buffer>& outVxBuffer, const uint8_t* aVertexDataPtr, size_t aNumVertices, size_t aVertexSize, bool aIsDynamic)
{
	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = static_cast<UINT>(aNumVertices * aVertexSize);
	vertexBufferDesc.Usage = aIsDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = aIsDynamic ? D3D11_CPU_ACCESS_WRITE : 0;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
	vertexSubresourceData.pSysMem = aVertexDataPtr;

	const HRESULT result = RHI::myDevice->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, outVxBuffer.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create vertex buffer!");
		return false;
	}
	SetObjectName(outVxBuffer, aName);

	return true;
}

bool RHI::UpdateConstantBufferInternal(const ConstantBuffer& aBuffer, const void* aBufferData, size_t aBufferDataSize) const
{
	if (!aBuffer.myBuffer)
	{
		LOG(RHILog, Error, "Failed to update constant buffer. Buffer {} is invalid.", aBuffer.myName);
		return false;
	}

	if (aBufferDataSize > aBuffer.myDataSize)
	{
		LOG(RHILog, Error, "Failed to update constant buffer. Data too large.");
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE bufferData = {};

	HRESULT result = myContext->Map(aBuffer.myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to map constant buffer {}.", aBuffer.myName);
		return false;
	}

	memcpy_s(bufferData.pData, aBufferDataSize, aBufferData, aBufferDataSize);
	myContext->Unmap(aBuffer.myBuffer.Get(), 0);

	return true;
}

bool RHI::UpdateVertexBufferInternal(const VertexBuffer& aVertexBuffer, const uint8_t* aBufferData, size_t aNumVertices, size_t aVertexSize) const
{
	if (!aVertexBuffer.Buffer || aVertexBuffer.VertexStride != aVertexSize || aVertexBuffer.DataSize != aNumVertices * aVertexSize)
	{
		LOG(RHILog, Error, "Failed to update vertex buffer {}.", aVertexBuffer.Name);
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE bufferData = {};

	HRESULT result = myContext->Map(aVertexBuffer.Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to map constant buffer {}.", aVertexBuffer.Name);
		return false;
	}

	memcpy_s(bufferData.pData, aVertexBuffer.DataSize, aBufferData, aVertexBuffer.DataSize);
	myContext->Unmap(aVertexBuffer.Buffer.Get(), 0);

	return true;

}




bool RHI::CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer)
{
	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.ByteWidth = static_cast<unsigned>(aIndexList.size() * sizeof(unsigned));
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubresourceData = {};
	indexSubresourceData.pSysMem = aIndexList.data();

	const HRESULT result = myDevice->CreateBuffer(&indexBufferDesc, &indexSubresourceData, outIxBuffer.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create index buffer - {}.", aName);
		return false;
	}

	SetObjectName(outIxBuffer, aName);

	return true;
}

bool RHI::CreateConstantBuffer(std::string_view aName, size_t aSize, unsigned aSlot, unsigned aPipelineStages, ConstantBuffer& outBuffer)
{
	if (aSize > 65536)
	{
		LOG(RHILog, Error, "Failed to create constant buffer {}. Size is larger than 64Kb ({}).", aName, aSize);
		return false;
	}

	outBuffer.myName = aName;
	outBuffer.myDataSize = aSize;
	outBuffer.myPipelineStages = aPipelineStages;
	outBuffer.mySlotIndex = aSlot;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = static_cast <unsigned>(aSize);

	const HRESULT result = myDevice->CreateBuffer(&bufferDesc, nullptr, outBuffer.myBuffer.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create constant buffer {}. Check DirectX log for more information", aName);
		return false;
	}

	SetObjectName(outBuffer.myBuffer, aName);
	LOG(RHILog, Log, "Created constant buffer {}.", aName);

	return true;
}

bool RHI::CreateSamplerState(std::string_view aName, const D3D11_SAMPLER_DESC& aDescription)
{
	ComPtr<ID3D11SamplerState> samplerState = {};
	const HRESULT result = myDevice->CreateSamplerState(&aDescription, samplerState.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create sampler state!");
		return false;
	}
	SetObjectName(samplerState, aName);
	std::string key(aName);
	mySamplerStates.emplace(key, samplerState);

	return true;
}

bool RHI::CreateInputLayout(std::string_view aName, Microsoft::WRL::ComPtr<ID3D11InputLayout>& outInputLayout, const std::vector<VertexElementDesc>& aInputLayoutDefinition, const uint8_t* aShaderDataPtr, size_t aShaderDataSize)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
	inputElements.reserve(aInputLayoutDefinition.size());

	for (const auto& vxED : aInputLayoutDefinition)
	{
		D3D11_INPUT_ELEMENT_DESC element = {};
		element.SemanticName = vxED.Semantic.data();
		element.SemanticIndex = vxED.SemanticIndex;
		element.Format = static_cast<DXGI_FORMAT>(vxED.Type);

		element.InputSlot = vxED.isInstanced ? 1:0;
		element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		element.InputSlotClass = vxED.isInstanced ? 
			D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
		element.InstanceDataStepRate = vxED.isInstanced ? 1 : 0;

		inputElements.emplace_back(element);
	}

	const HRESULT result = myDevice->CreateInputLayout(
		inputElements.data(),
		static_cast<unsigned>(inputElements.size()),
		aShaderDataPtr,
		aShaderDataSize,
		outInputLayout.GetAddressOf()
	);
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create input layout");
		return false;
	}

	SetObjectName(outInputLayout, aName);

	return true;
}

bool RHI::LoadTexture(TextureAsset* outTexture, const std::filesystem::path& aFileName)
{
	assert(outTexture && "Please initialize the Texture Object before calling this function!");
	outTexture->myName = aFileName.stem().string().c_str();
	outTexture->myBindFlags = D3D11_BIND_SHADER_RESOURCE;
	outTexture->myUsageFlags = D3D11_USAGE_DEFAULT;
	outTexture->myAccessFlags = 0;

	HRESULT result = DirectX::CreateDDSTextureFromFile(
		myDevice.Get(),
		aFileName.wstring().c_str(),
		outTexture->myTexture.GetAddressOf(),
		outTexture->mySRV.GetAddressOf()
	);

	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to load the requested texture! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
		return false;
	}

	result = myDevice->CreateShaderResourceView(outTexture->myTexture.Get(), nullptr, outTexture->mySRV.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create a shader resource view! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
		return false;
	}

	std::string textureName = aFileName.string();
	if (const size_t pos = textureName.find_last_of(L'\\'); pos != std::wstring::npos)
	{
		textureName = textureName.substr(pos + 1);
	}

	textureName = textureName.substr(0, textureName.size() - 4);
	outTexture->myName = textureName.c_str();



	return true;
}

//bool RHI::CreateVertexShader(ComPtr<ID3D11VertexShader>& outVxShader, const BYTE* someShaderData,
//	size_t aShaderDataSize)
//{
//	const HRESULT result = myDevice->CreateVertexShader(someShaderData, aShaderDataSize, nullptr, &outVxShader);
//	if (FAILED(result))
//	{
//		ReportError(result, "Failed to load vertex shader from the specified file!");
//		return false;
//	}
//
//	return true;
//}

//bool RHI::CreatePixelShader(ComPtr<ID3D11PixelShader>& outPxShader, const BYTE* someShaderData, size_t aShaderDataSize)
//{
//	const HRESULT result = myDevice->CreatePixelShader(someShaderData, aShaderDataSize, nullptr, outPxShader.GetAddressOf());
//	if (FAILED(result))
//	{
//		ReportError(result, "Failed to load pixel shader from the specified file!");
//		return false;
//	}
//
//	return true;
//}

//void RHI::ReflectShaderVariableMember(const std::wstring& aDomain, size_t& anOffset,
//	ID3D11ShaderReflectionType* aMemberType, const D3D11_SHADER_TYPE_DESC* aMemberTypeDesc,
//	ShaderInfo::ConstantBufferInfo& inoutBufferInfo)
//{
//	if (aMemberTypeDesc->Members > 0)
//	{
//		for (unsigned m = 0; m < aMemberTypeDesc->Members; m++)
//		{
//			ID3D11ShaderReflectionType* childMemberType = aMemberType->GetMemberTypeByIndex(m);
//
//			D3D11_SHADER_TYPE_DESC childMemberTypeDesc;
//			ZeroMemory(&childMemberTypeDesc, sizeof(D3D11_SHADER_TYPE_DESC));
//			childMemberType->GetDesc(&childMemberTypeDesc);
//
//			const std::wstring childMemberName = string_cast<std::wstring>(aMemberType->GetMemberTypeName(m));
//			const std::wstring newDomain = aDomain.empty() ? childMemberName : aDomain + L"." + childMemberName;
//
//			ReflectShaderVariableMember(newDomain, anOffset, childMemberType, &childMemberTypeDesc, inoutBufferInfo);
//		}
//	}
//	else
//	{
//		ShaderInfo::VariableInfo varInfo;
//		varInfo.Name = aDomain;
//		varInfo.Type = string_cast<std::wstring>(aMemberTypeDesc->Name);
//		varInfo.Size = static_cast<unsigned long long>(aMemberTypeDesc->Columns) *
//			static_cast<unsigned long long>(aMemberTypeDesc->Rows) * sizeof(float);
//		varInfo.Offset = anOffset;
//		anOffset += varInfo.Size;
//		inoutBufferInfo.VariableNameToIndex.emplace(varInfo.Name, inoutBufferInfo.Variables.size());
//		inoutBufferInfo.Variables.emplace_back(varInfo);
//	}
//}

//void RHI::ReflectShaderVariable(const std::wstring& aDomain, size_t& anOffset, ID3D11ShaderReflectionVariable* aVariable, ShaderInfo::ConstantBufferInfo& inoutBufferInfo)
//{
//	aDomain;
//	// Get all the info about this particular variable.
//	// I.e. its description...
//	D3D11_SHADER_VARIABLE_DESC variableDesc;
//	ZeroMemory(&variableDesc, sizeof(D3D11_SHADER_VARIABLE_DESC));
//	aVariable->GetDesc(&variableDesc);
//	// ...and it's type object and its description.
//	ID3D11ShaderReflectionType* variableType = aVariable->GetType();
//	D3D11_SHADER_TYPE_DESC variableTypeDesc;
//	ZeroMemory(&variableTypeDesc, sizeof(D3D11_SHADER_TYPE_DESC));
//	variableType->GetDesc(&variableTypeDesc);
//
//	// Now we should know what this is.
//	if (variableTypeDesc.Members > 0)
//	{
//		// Start a Domain for this Variable
//		const std::wstring memberDomain = string_cast<std::wstring>(variableDesc.Name);
//		ReflectShaderVariableMember(memberDomain, anOffset, variableType, &variableTypeDesc, inoutBufferInfo);
//	}
//	else
//	{
//		ShaderInfo::VariableInfo varInfo;
//		varInfo.Name = string_cast<std::wstring>(variableDesc.Name);
//		varInfo.Type = string_cast<std::wstring>(variableTypeDesc.Name);
//		varInfo.Size = variableDesc.Size;
//		varInfo.Offset = anOffset;
//		anOffset += variableDesc.Size;
//		inoutBufferInfo.VariableNameToIndex.emplace(varInfo.Name, inoutBufferInfo.Variables.size());
//		inoutBufferInfo.Variables.emplace_back(varInfo);
//	}
//}

//bool RHI::ReflectShader(ShaderInfo& outShRefl, const BYTE* someShaderData, size_t aShaderDataSize)
//{
//	ComPtr<ID3D11ShaderReflection> Data;
//	const HRESULT result = D3DReflect(someShaderData, aShaderDataSize, IID_ID3D11ShaderReflection, reinterpret_cast<void**>(Data.GetAddressOf()));
//	if (FAILED(result))
//	{
//		ReportError(result, "Failed to reflect the provided shader!");
//		return false;
//	}
//
//	ComPtr<ID3D11ShaderReflection> shaderReflection = Data;
//	D3D11_SHADER_DESC shaderDesc;
//	ZeroMemory(&shaderDesc, sizeof(D3D11_SHADER_DESC));
//	shaderReflection->GetDesc(&shaderDesc);
//
//	outShRefl.InstructionCount = shaderDesc.InstructionCount;
//	outShRefl.ConstantBuffers.reserve(shaderDesc.ConstantBuffers);
//	outShRefl.Type = static_cast<ShaderType>(D3D11_SHVER_GET_TYPE(shaderDesc.Version));
//
//	if (outShRefl.Type == ShaderType::VertexShader)
//	{
//		//// We need to extract the Input Layout too.
//		//std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
//		//for(unsigned p = 0; p < shaderDesc.InputParameters; p++)
//		//{
//		//	D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
//		//	shaderReflection->GetInputParameterDesc(p, &paramDesc);
//
//		//	D3D11_INPUT_ELEMENT_DESC element;
//		//	element.SemanticName = paramDesc.SemanticName;
//		//	element.SemanticIndex = paramDesc.SemanticIndex;
//		//	element.InputSlot = paramDesc.Register;
//		//	element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
//		//	element.InputSlotClass = paramDesc.
//		//}
//	}
//
//	for (unsigned c = 0; c < shaderDesc.ConstantBuffers; c++)
//	{
//		ShaderInfo::ConstantBufferInfo bufferInfo;
//		ID3D11ShaderReflectionConstantBuffer* cbufferReflection = shaderReflection->GetConstantBufferByIndex(c);
//
//		D3D11_SHADER_BUFFER_DESC cbufferDesc;
//		D3D11_SHADER_INPUT_BIND_DESC cbufferBindDesc;
//		ZeroMemory(&cbufferDesc, sizeof(D3D11_SHADER_BUFFER_DESC));
//		ZeroMemory(&cbufferBindDesc, sizeof(D3D11_SHADER_INPUT_BIND_DESC));
//
//		cbufferReflection->GetDesc(&cbufferDesc);
//		shaderReflection->GetResourceBindingDescByName(cbufferDesc.Name, &cbufferBindDesc);
//
//		bufferInfo.Size = cbufferDesc.Size;
//		std::string bufferName = cbufferDesc.Name;
//		bufferInfo.Name = string_cast<std::wstring>(bufferName);
//		bufferInfo.Slot = cbufferBindDesc.BindPoint;
//
//		bufferInfo.Variables.reserve(cbufferDesc.Variables);
//		bufferInfo.VariableNameToIndex.reserve(cbufferDesc.Variables);
//		size_t dataOffset = 0;
//		for (unsigned v = 0; v < cbufferDesc.Variables; v++)
//		{
//			ID3D11ShaderReflectionVariable* varReflection = cbufferReflection->GetVariableByIndex(v);
//			ReflectShaderVariable(L"", dataOffset, varReflection, bufferInfo);
//		}
//
//		outShRefl.ConstantBufferNameToIndex.emplace(bufferInfo.Name, outShRefl.ConstantBuffers.size());
//		outShRefl.ConstantBuffers.emplace_back(std::move(bufferInfo));
//	}
//
//	for (unsigned r = 0; r < shaderDesc.BoundResources; r++)
//	{
//		D3D11_SHADER_INPUT_BIND_DESC resourceBindDesc;
//		ZeroMemory(&resourceBindDesc, sizeof(D3D11_SHADER_INPUT_BIND_DESC));
//		shaderReflection->GetResourceBindingDesc(r, &resourceBindDesc);
//		switch (resourceBindDesc.Type)
//		{
//		case D3D_SIT_TEXTURE:
//		{
//			ShaderInfo::TextureInfo textureInfo;
//			textureInfo.Name = string_cast<std::wstring>(resourceBindDesc.Name);
//			textureInfo.Slot = resourceBindDesc.BindPoint;
//			switch (resourceBindDesc.Dimension)
//			{
//			case D3D_SRV_DIMENSION_TEXTURE1D:
//			{
//				textureInfo.Type = TextureType::Texture1D;
//			}
//			case D3D_SRV_DIMENSION_TEXTURE2D:
//			{
//				textureInfo.Type = TextureType::Texture2D;
//			}
//			case D3D_SRV_DIMENSION_TEXTURE3D:
//			{
//				textureInfo.Type = TextureType::Texture3D;
//			}
//			default:
//			{
//				textureInfo.Type = TextureType::Unknown;
//			}
//			}
//
//			outShRefl.TextureNameToIndex.emplace(textureInfo.Name, outShRefl.Textures.size());
//			outShRefl.Textures.emplace_back(std::move(textureInfo));
//		}
//		break;
//		case D3D_SIT_SAMPLER:
//		{
//			ShaderInfo::SamplerInfo samplerInfo;
//			samplerInfo.Name = string_cast<std::wstring>(resourceBindDesc.Name);
//			samplerInfo.Slot = resourceBindDesc.BindPoint;
//
//			outShRefl.SamplerNameToIndex.emplace(samplerInfo.Name, outShRefl.Samplers.size());
//			outShRefl.Samplers.emplace_back(std::move(samplerInfo));
//		}
//		break;
//		default:
//			break;
//		}
//	}
//
//	return true;
//}

//bool RHI::CreateGeometryShader(ComPtr<ID3D11GeometryShader>& outGeoShader, const BYTE* someShaderData, size_t aShaderDataSize)
//{
//	const HRESULT result = myDevice->CreateGeometryShader(someShaderData, aShaderDataSize, nullptr, outGeoShader.GetAddressOf());
//	if (FAILED(result))
//	{
//		ReportError(result, "Failed to load geometry shader from the specified file!");
//		return false;
//	}
//
//	return true;
//}
//
//bool RHI::LoadPixelShader(ComPtr<ID3D11PixelShader>& outPxShader, const std::wstring& aFileName)
//{
//	std::ifstream psFile;
//	psFile.open(aFileName, std::ios::binary);
//	const std::vector<uint8_t> psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
//	psFile.close();
//
//	return CreatePixelShader(outPxShader, psData.data(), psData.size());
//}
//


bool RHI::LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize, TextureAsset& outTexture) const
{
	ComPtr<ID3D11Resource> resource;
	const HRESULT result = DirectX::CreateDDSTextureFromMemory(myDevice.Get(), aTextureDataPtr,
		aTextureDataSize, &resource, &outTexture.mySRV);

	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to load texture {}", aName);
	}

	D3D11_RESOURCE_DIMENSION resourceDimension = {};
	resource->GetType(&resourceDimension);
	switch (resourceDimension)
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
	{
		ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);

		D3D11_TEXTURE2D_DESC desc = {};
		texture->GetDesc(&desc);
		outTexture.myBindFlags = desc.BindFlags;
		outTexture.myAccessFlags = desc.CPUAccessFlags;
		outTexture.myUsageFlags = desc.Usage;
		outTexture.mySize = desc.ArraySize;
		outTexture.myTexture = texture;
	}
	break;

	default:
		LOG(RHILog, Error, "Wrong dimension format on texture {}", aName);
		break;
	}

	//Turn off warnings for renaming objects
	Microsoft::WRL::ComPtr<ID3D11Debug> deviceDebug;
	myDevice.As(&deviceDebug);
	if (deviceDebug)
	{
		Microsoft::WRL::ComPtr<ID3D11InfoQueue> infoQueue;
		deviceDebug->QueryInterface(IID_PPV_ARGS(&infoQueue));

		D3D11_MESSAGE_ID mask[] = {
			D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS
		};

		D3D11_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(mask);
		filter.DenyList.pIDList = mask;
		infoQueue->AddStorageFilterEntries(&filter);
	}

	SetObjectName(resource, aName);
	std::string srvName(aName);
	srvName.append("_SRV");
	SetObjectName(outTexture.mySRV, srvName);

	return true;
}

//bool RHI::LoadShader(Shader* outShader, const std::wstring& aFileName)
//{
//	assert(outShader && "Please initialize the Shader Object before calling this function!");
//	std::ifstream shFile;
//	shFile.open(aFileName, std::ios::binary);
//	const std::vector<uint8_t> shData = { std::istreambuf_iterator<char>(shFile), std::istreambuf_iterator<char>() };
//	shFile.close();
//
//	std::wstring shaderName = aFileName;
//	if (const size_t pos = shaderName.find_last_of(L'\\'); pos != std::wstring::npos)
//	{
//		shaderName = shaderName.substr(pos + 1);
//	}
//
//	shaderName = shaderName.substr(0, shaderName.size() - 4);
//
//	return LoadShaderFromMemory(outShader, shaderName, shData.data(), shData.size());
//}
//
//bool RHI::LoadShaderFromMemory(Shader* outShader, const std::wstring& aName, const BYTE* someShaderData,
//	size_t aShaderDataSize)
//{
//	assert(outShader && "Please initialize the Shader Object before calling this function!");
//	outShader->myName = aName;
//	outShader->myBlob = new unsigned char[aShaderDataSize];
//	outShader->myBlobSize = aShaderDataSize;
//	memcpy_s(outShader->myBlob, outShader->myBlobSize, someShaderData, aShaderDataSize);
//
//	if (ReflectShader(outShader->myShaderInfo, outShader->myBlob, outShader->myBlobSize))
//	{
//		switch (outShader->GetShaderType())
//		{
//		case ShaderType::VertexShader:
//		{
//			ComPtr<ID3D11VertexShader> vsShader;
//			CreateVertexShader(vsShader, outShader->myBlob, outShader->myBlobSize);
//			outShader->myShaderObject = vsShader;
//		}
//		break;
//		case ShaderType::PixelShader:
//		{
//			ComPtr<ID3D11PixelShader> psShader;
//			CreatePixelShader(psShader, outShader->myBlob, outShader->myBlobSize);
//			outShader->myShaderObject = psShader;
//		}
//		break;
//		case ShaderType::GeometryShader:
//		{
//			ComPtr<ID3D11GeometryShader> gsShader;
//			CreateGeometryShader(gsShader, outShader->myBlob, outShader->myBlobSize);
//			outShader->myShaderObject = gsShader;
//		}
//		break;
//		}
//
//		return true;
//	}
//
//	return false;
//}

bool RHI::LoadShaderFromMemory(std::string_view aName, Shader& outShader, const uint8_t* aShaderDataPtr, size_t aShaderDataSize)
{
	ComPtr<ID3D11ShaderReflection> shaderReflection;
	HRESULT result = D3DReflect(
		aShaderDataPtr,
		aShaderDataSize,
		IID_ID3D11ShaderReflection,
		reinterpret_cast<void**>(shaderReflection.GetAddressOf())
	);
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to load shader");
		return false;
	}

	D3D11_SHADER_DESC shaderDesc = {};
	shaderReflection->GetDesc(&shaderDesc);

	D3D11_SHADER_VERSION_TYPE shaderVersion = static_cast<D3D11_SHADER_VERSION_TYPE>(D3D11_SHVER_GET_TYPE(shaderDesc.Version));
	switch (shaderVersion)
	{
	case D3D11_SHVER_VERTEX_SHADER:
	{
		outShader.type = ShaderType::VertexShader;
		ComPtr<ID3D11VertexShader> vsShader;
		result = myDevice->CreateVertexShader(aShaderDataPtr, aShaderDataSize, nullptr, &vsShader);
		if (FAILED(result))
		{
			LOG(RHILog, Error, "Failed to create vertex shader");
			return false;
		}
		outShader.shader = vsShader;
		break;
	}
	case D3D11_SHVER_PIXEL_SHADER:
	{
		outShader.type = ShaderType::PixelShader;
		ComPtr<ID3D11PixelShader> psShader;
		result = myDevice->CreatePixelShader(aShaderDataPtr, aShaderDataSize, nullptr, &psShader);
		if (FAILED(result))
		{
			LOG(RHILog, Error, "Failed to create pixel shader");
			return false;
		}
		outShader.shader = psShader;
		break;
	}
	case D3D11_SHVER_GEOMETRY_SHADER:
	{
		outShader.type = ShaderType::GeometryShader;
		ComPtr<ID3D11GeometryShader> gsShader;
		result = myDevice->CreateGeometryShader(aShaderDataPtr, aShaderDataSize, nullptr, &gsShader);
		if (FAILED(result))
		{
			LOG(RHILog, Error, "Failed to create geometry shader");
			return false;
		}
		outShader.shader = gsShader;
		break;
	}
	}

	SetObjectName(outShader.shader, aName);

	return true;
}

bool RHI::LoadTextureFromMemory(TextureAsset* outTexture, const std::filesystem::path& aName, const BYTE* someImageData, size_t anImageDataSize, const D3D11_SHADER_RESOURCE_VIEW_DESC* aSRVDesc)
{
	assert(outTexture && "Please initialize the Texture Object before calling this function!");
	outTexture->myName = aName.string().c_str();
	outTexture->myBindFlags = D3D11_BIND_SHADER_RESOURCE;
	outTexture->myUsageFlags = D3D11_USAGE_DEFAULT;
	outTexture->myAccessFlags = 0;

	HRESULT result = DirectX::CreateDDSTextureFromMemory(
		myDevice.Get(),
		someImageData,
		anImageDataSize,
		outTexture->myTexture.GetAddressOf(),
		outTexture->mySRV.GetAddressOf()
	);

	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to load the requested texture! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
		return false;
	}

	result = myDevice->CreateShaderResourceView(outTexture->myTexture.Get(), aSRVDesc, outTexture->mySRV.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create a shader resource view! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
		return false;
	}

	return true;
}

void RHI::ConfigureInputAssembler(unsigned aTopology, const ComPtr<ID3D11Buffer>& aVxBuffer, const ComPtr<ID3D11Buffer>& anIxBuffer,
	unsigned aVertexStride, const ComPtr<ID3D11InputLayout>& anInputLayout)
{
	myContext->IASetInputLayout(anInputLayout.Get());
	myContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(aTopology));
	constexpr unsigned vxOffset = 0;
	myContext->IASetVertexBuffers(0, 1, aVxBuffer.GetAddressOf(), &aVertexStride, &vxOffset);
	DXGI_FORMAT indexBufferFormat = DXGI_FORMAT_R32_UINT;
	if (anIxBuffer == nullptr)
	{
		indexBufferFormat = DXGI_FORMAT_UNKNOWN;
	}
	myContext->IASetIndexBuffer(anIxBuffer.Get(), indexBufferFormat, 0);
}

void RHI::SetVertexShader(const ComPtr<ID3D11VertexShader>& aVertexShader)
{
	myContext->VSSetShader(aVertexShader.Get(), nullptr, 0);
}

void RHI::SetVertexShader(std::shared_ptr<Shader> aVertexShader)
{
	ComPtr<ID3D11VertexShader> vsShader;
	aVertexShader->shader.As(&vsShader);
	if (aVertexShader)
	{
		aVertexShader->shader.As(&vsShader);
	}
	myContext->VSSetShader(vsShader.Get(), nullptr, 0);
}


void RHI::SetPixelShader(std::shared_ptr<Shader> aPixelShader)
{
	ComPtr<ID3D11PixelShader> psShader = nullptr;
	if (aPixelShader)
	{
		aPixelShader->shader.As(&psShader);
		myContext->PSSetShader(psShader.Get(), nullptr, 0);
	}
	else
	{
		myContext->PSSetShader(nullptr, nullptr, 0);
	}
}

void RHI::SetGeometryShader(std::shared_ptr<Shader> aGeometryShader)
{
	ComPtr<ID3D11GeometryShader> gShader = nullptr;
	if (aGeometryShader)
	{
		aGeometryShader->shader.As(&gShader);
		myContext->GSSetShader(gShader.Get(), nullptr, 0);
	}
	else
	{
		myContext->GSSetShader(nullptr, nullptr, 0);
	}
}

void RHI::SetRenderTarget(std::shared_ptr<TextureAsset> aRenderTarget) const
{
	if (aRenderTarget)
	{
		myContext->OMSetRenderTargets(1, aRenderTarget->GetRTV().GetAddressOf(), aRenderTarget->GetDSV().Get());
	}
	else
	{
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> nullRTV;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> nullDSV;
		myContext->OMSetRenderTargets(1, nullRTV.GetAddressOf(), nullDSV.Get());
	}
}


void RHI::SetRasterizerState(const RasterizerState aState)
{
	myContext->RSSetState(myRasterizerStates[aState].Get());
}

void RHI::SetDepthState(const DepthState aState)
{
	myContext->OMSetDepthStencilState(myDepthStates[aState].Get(), 0);
}

//void RHI::SetConstantBuffer(UINT aPipelineStages, unsigned aSlot, const ConstantBufferBase& aBuffer)
//{
//	if (aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
//	{
//		myContext->VSSetConstantBuffers(aSlot, 1, aBuffer.myBuffer.GetAddressOf());
//	}
//
//	if (aPipelineStages & PIPELINE_STAGE_GEOMETRY_SHADER)
//	{
//		myContext->GSSetConstantBuffers(aSlot, 1, aBuffer.myBuffer.GetAddressOf());
//	}
//
//	if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
//	{
//		myContext->PSSetConstantBuffers(aSlot, 1, aBuffer.myBuffer.GetAddressOf());
//	}
//}

void RHI::SetObjectName(ComPtr<ID3D11DeviceChild> anObject, std::string_view aName) const
{
	if (anObject)
	{
		anObject->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<unsigned>(sizeof(char) * aName.size()), aName.data());
	}
}


void RHI::DrawIndexed(unsigned aStartIndex, unsigned aIndexCount) const
{
	myContext->DrawIndexed(aIndexCount, aStartIndex, 0);
}

void RHI::DrawIndexedInstanced(unsigned anIndexCount, unsigned anInstanceCount)
{
	myContext->DrawIndexedInstanced(anIndexCount, anInstanceCount, 0, 0, 0);
}

void RHI::Draw(unsigned aCount)
{
	myContext->Draw(aCount, 0);
}

void RHI::ClearRenderTargets(const int& aNumViews) const
{
	myContext->OMSetRenderTargets(aNumViews - 1, nullptr, nullptr);
}

void RHI::ClearBackBuffer() const
{
	FLOAT clearColor[4] = { 0, 0, 0, 0 };
	myContext->ClearRenderTargetView(myBackBuffer->myRTV.Get(), clearColor);
}


bool RHI::CreateTexture(std::string aName, unsigned aWidth, unsigned aHeight, RHITextureFormat aFormat, TextureAsset* outTexture, bool aStaging, bool aShaderResource, bool aRenderTarget, bool aCpuAccessRead, bool aCpuAccessWrite) const
{
	// If this texture is a Staging texture or not.
	// Staging textures cannot be rendered to but are useful in i.e. screen picking.
	const unsigned usage = aStaging ? D3D11_USAGE_STAGING : D3D11_USAGE_DEFAULT;

	// Bind flags for this texture.
	// I.e. if it's used as a Resource (read)
	// or a Target (write)
	unsigned bindFlags = {};
	if (aShaderResource)
	{
		bindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}

	if (aRenderTarget)
	{
		bindFlags |= D3D11_BIND_RENDER_TARGET;
	}

	// CPU flags letting us read and/or write
	// from/to a texture. Imposes several restrictions.
	// Primary use in TGP is if you implement picking.
	unsigned cpuFlags = {};
	if (aCpuAccessRead)
	{
		cpuFlags |= D3D11_CPU_ACCESS_READ;
	}
	if (aCpuAccessWrite)
	{
		cpuFlags |= D3D11_CPU_ACCESS_WRITE;
	}

	// These might differ for you!
	outTexture->myBindFlags = bindFlags;
	outTexture->myAccessFlags = cpuFlags;
	outTexture->myUsageFlags = static_cast<D3D11_USAGE>(usage);

	HRESULT result = E_FAIL;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = aWidth;
	desc.Height = aHeight;
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.Format = static_cast<DXGI_FORMAT>(aFormat);
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = static_cast<D3D11_USAGE>(usage);
	desc.BindFlags = bindFlags;
	desc.CPUAccessFlags = cpuFlags;
	desc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> texture;
	result = myDevice->CreateTexture2D(&desc, nullptr, reinterpret_cast<ID3D11Texture2D**>(texture.GetAddressOf()));
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create the texture {}!", aName);
		return false;
	}

	SetObjectName(texture, aName);

	if (aShaderResource)
	{
		result = myDevice->CreateShaderResourceView(texture.Get(), nullptr, &outTexture->mySRV);
		if (FAILED(result))
		{
			LOG(RHILog, Error, "Failed to create the texture {}! Failed to create a shader resource view!", aName);
			return false;
		}

		SetObjectName(outTexture->mySRV, aName + "_SRV");
	}

	if (aRenderTarget)
	{
		result = myDevice->CreateRenderTargetView(texture.Get(), nullptr, &outTexture->myRTV);
		if (FAILED(result))
		{
			LOG(RHILog, Error, "Failed to create the texture {}! Failed to create a render target view!", aName);
			return false;
		}

		SetObjectName(outTexture->myRTV, aName + "_RTV");
	}
	outTexture->myName = aName;

	return true;
}

bool RHI::CreateSRV(TextureAsset* outTexture, std::string_view aName)
{
	HRESULT result = E_FAIL;
	result = myDevice->CreateShaderResourceView(outTexture->myTexture.Get(), nullptr, &outTexture->mySRV);
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create the texture {}! Failed to create a shader resource view!", aName);
		return false;
	}

	std::string name(aName);
	SetObjectName(outTexture->mySRV, name + "_SRV");

	//myDevice->CreateShaderResourceView()
	return false;
}


void RHI::ClearRenderTarget(const TextureAsset* aTexture, std::array<float, 4> aClearColor)
{
	if ((aTexture->myBindFlags & D3D11_BIND_RENDER_TARGET) == false)
	{
		std::throw_with_nested(std::runtime_error("Attempted to clear a read-only texture!"));
	}
	myContext->ClearRenderTargetView(aTexture->myRTV.Get(), aClearColor.data());
}

void RHI::ClearDepthStencil(const TextureAsset* aTexture)
{
	if ((aTexture->myBindFlags & D3D11_BIND_DEPTH_STENCIL) == false)
	{
		std::throw_with_nested(std::runtime_error("Attempted to clear depth on a non-depth texture!"));
	}

	RHI::myContext->ClearDepthStencilView(aTexture->myDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

bool RHI::SetTextureResource(unsigned aPipelineStages, unsigned aSlot, const TextureAsset& aTexture) const
{
	if (!(aTexture.myBindFlags & D3D11_BIND_SHADER_RESOURCE))
	{
		LOG(RHILog, Error, "Couldn't set texture resource");
		return false;
	}

	if (aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
	{
		myContext->VSSetShaderResources(aSlot, 1, aTexture.mySRV.GetAddressOf());
	}
	if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
	{
		myContext->PSSetShaderResources(aSlot, 1, aTexture.mySRV.GetAddressOf());
	}

	return true;
}

bool RHI::ClearTextureResourceSlot(unsigned aPipelineStages, unsigned aSlot) const
{
	ID3D11ShaderResourceView* dummy[] = { nullptr };
	if (aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
	{
		myContext->VSSetShaderResources(aSlot, 1, dummy);
	}
	if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
	{
		myContext->PSSetShaderResources(aSlot, 1, dummy);
	}

	return true;

}

void RHI::BeginEvent(std::string_view anEventName)
{
	const std::wstring markerW = str::utf8_to_wide(anEventName.data());
	myAnnotationObject->BeginEvent(markerW.c_str());
}

void RHI::EndEvent()
{
	myAnnotationObject->EndEvent();
}

void RHI::SetMarker(std::string_view aMarker)
{
	const std::wstring markerW = str::utf8_to_wide(aMarker.data());
	myAnnotationObject->SetMarker(markerW.c_str());
}

void RHI::DrawQuad(const CU::Vector2f& aSize)
{
	SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	SetVertexBuffer(nullptr, 0, 0);
	SetIndexBuffer(nullptr);
	SetInputLayout(nullptr);

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = aSize.x > 0 ? aSize.x : myBackBuffer->GetSize().x;
	viewport.Height = aSize.y > 0 ? aSize.y : myBackBuffer->GetSize().y;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	myContext->RSSetViewports(1, &viewport);

	Draw(4);
}

void RHI::ChangePipeLineState(const PipelineStateObject& aNewPSO, const PipelineStateObject& anOldPSO)
{
	for (int i = 0; i < anOldPSO.RenderTargetCount; i++)
	{
		if (anOldPSO.RenderTarget[i])
		{
			ID3D11RenderTargetView* nullRTV = nullptr;
			myContext->OMSetRenderTargets(1, &nullRTV, nullptr);
		}
		else
		{
			if (anOldPSO.DepthStencil)
			{
				myContext->OMSetRenderTargets(0, nullptr, nullptr);
				break;
			}
		}
	}

	const std::array<float, 4> blendFactor = { 0, 0, 0, 0 };
	constexpr unsigned samplerMask = 0xffffffff;
	myContext->OMSetBlendState(aNewPSO.BlendState.Get(), blendFactor.data(), samplerMask);
	myContext->RSSetState(aNewPSO.RasterizerState.Get());
	myContext->OMSetDepthStencilState(aNewPSO.DepthStencilState.Get(), 0);

	for (const auto& [slot, sampler] : aNewPSO.SamplerStates)
	{
		myContext->VSSetSamplers(slot, 1, sampler.GetAddressOf());
		myContext->PSSetSamplers(slot, 1, sampler.GetAddressOf());
	}


	constexpr int size = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
	ID3D11RenderTargetView* renderViews[size] = {};

	for (size_t i = 0; i < aNewPSO.RenderTargetCount; i++)
	{
		if (aNewPSO.ClearRenderTarget[i])
		{
			myContext->ClearRenderTargetView(aNewPSO.RenderTarget[i]->myRTV.Get(), aNewPSO.RenderTarget[i]->myClearColor.data());
		}
	}

	if (aNewPSO.ClearDepthStencil)
	{
		if (aNewPSO.DepthStencil)
		{
			myContext->ClearDepthStencilView(aNewPSO.DepthStencil->myDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
	}

	for (int i = 0; i < aNewPSO.RenderTargetCount; i++)
	{
		if (aNewPSO.RenderTarget[i])
		{
			renderViews[i] = *aNewPSO.RenderTarget[i]->myRTV.GetAddressOf();
		}
	}

	D3D11_VIEWPORT viewports[size] = {};
	for (int i = 0; i < aNewPSO.RenderTargetCount; i++)
	{
		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = myBackBuffer->myViewPort.Width;
		viewport.Height = myBackBuffer->myViewPort.Height;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		viewports[i] = viewport;
	}

	ID3D11DepthStencilView* nullDSV[] = { nullptr };
	ID3D11RenderTargetView* nullRTV = nullptr;




	if (aNewPSO.RenderTargetCount > 0)
	{
		if (aNewPSO.Type == PSOType::Sprite)
		{
			myContext->OMSetRenderTargets(aNewPSO.RenderTargetCount, renderViews, *nullDSV);
		}
		else
			if (aNewPSO.DepthStencil)
			{
				myContext->OMSetRenderTargets(aNewPSO.RenderTargetCount, renderViews, aNewPSO.DepthStencil->myDSV.Get());
			}
			else
			{
				myContext->OMSetRenderTargets(aNewPSO.RenderTargetCount, renderViews, *nullDSV);
			}
		myContext->RSSetViewports(aNewPSO.RenderTargetCount, viewports);
	}
	else
	{

		if (aNewPSO.DepthStencil != nullptr)
		{
			myContext->OMSetRenderTargets(1, &nullRTV, aNewPSO.DepthStencil->myDSV.Get());
		}
		else
		{
			myContext->OMSetRenderTargets(1, &nullRTV, *nullDSV);
		}

		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		if (aNewPSO.DepthStencil != nullptr)
		{
			viewport.Width = aNewPSO.DepthStencil->myViewPort.Width;
			viewport.Height = aNewPSO.DepthStencil->myViewPort.Height;
		}
		else
		{
			viewport.Width = myBackBuffer->myViewPort.Width;
			viewport.Height = myBackBuffer->myViewPort.Height;
		}

		myContext->RSSetViewports(1, &viewport);
	}
}

void RHI::ChangeDepthStencil(const std::shared_ptr<TextureAsset> aDepthTexture)
{
	ID3D11RenderTargetView* nullRTV = nullptr;
	ID3D11DepthStencilView* nullDSV[] = { nullptr };

	if (aDepthTexture.get() != nullptr)
	{
		myContext->OMSetRenderTargets(1, &nullRTV, aDepthTexture->myDSV.Get());
	}
	else
	{
		myContext->OMSetRenderTargets(1, &nullRTV, *nullDSV);
	}
}

bool RHI::CreateShadowMap(const std::string& aName, CommonUtilities::Vector2<unsigned> aSize, TextureAsset& aOutTexture)
{
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = aSize.x;
	depthDesc.Height = aSize.y;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	HRESULT result;
	ComPtr<ID3D11Texture2D> depthTexture = {};
	result = myDevice->CreateTexture2D(&depthDesc, nullptr, depthTexture.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Couldn't create 2D texture in ShadowMap {}", aName);
		return false;
	}

	std::string baseName(aName);
	SetObjectName(depthTexture, baseName);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	result = myDevice->CreateDepthStencilView(depthTexture.Get(), &dsvDesc, aOutTexture.myDSV.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Couldn't create depth stencil view in ShadowMap {}", aName);
		return false;
	}
	SetObjectName(aOutTexture.myDSV, baseName + "_DSV");
	std::filesystem::path name = aName;
	aOutTexture.SetName(name.string().c_str());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = depthDesc.MipLevels;

	result = myDevice->CreateShaderResourceView(depthTexture.Get(), &srvDesc, &aOutTexture.mySRV);
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Couldn't create shader resource view in ShadowMap {}", aName);
		return false;
	}
	SetObjectName(aOutTexture.mySRV, baseName + "_SRV");

	aOutTexture.myAccessFlags = depthDesc.CPUAccessFlags;
	aOutTexture.myUsageFlags = depthDesc.Usage;
	aOutTexture.mySize = depthDesc.ArraySize;
	aOutTexture.myBindFlags = depthDesc.BindFlags;
	aOutTexture.myViewPort = { 0.f, 0.f, static_cast<float>(aSize.x), static_cast<float>(aSize.y), 1.f, 1.f };
	return true;
}

bool RHI::CreateLUTTexture(TextureAsset& aTexture, const std::shared_ptr<Shader>& aBrdfPixelShader, const std::shared_ptr<Shader>& aQuadVertexShader)
{
	BeginEvent("LUT");
	if (!CreateTexture("LUTTexture", 512, 512, RHITextureFormat::R16G16_Float, &aTexture, false, true, true, false, false))
	{
		LOG(RHILog, Error, "Failed to create LUTTexture");
		return false;
	}
	myContext->OMSetRenderTargets(1, aTexture.myRTV.GetAddressOf(), nullptr);
	D3D11_SAMPLER_DESC lutSamplerDesc = {};
	lutSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	lutSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutSamplerDesc.MipLODBias = 0.0f;
	lutSamplerDesc.MaxAnisotropy = 1;
	lutSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	lutSamplerDesc.BorderColor[0] = 0.0f;
	lutSamplerDesc.BorderColor[1] = 0.0f;
	lutSamplerDesc.BorderColor[2] = 0.0f;
	lutSamplerDesc.BorderColor[3] = 0.0f;
	lutSamplerDesc.MinLOD = 0;
	lutSamplerDesc.MaxLOD = 0;

	if (!CreateSamplerState("LUTSampler", lutSamplerDesc))
	{
		LOG(RHILog, Error, "Failed to create lut sampler");
	}
	myContext->VSSetSamplers(14, 1, mySamplerStates.at("LUTSampler").GetAddressOf());
	myContext->PSSetSamplers(14, 1, mySamplerStates.at("LUTSampler").GetAddressOf());
#include "CompiledHeaders\BRDF_PS.h"

	Shader psShader = *aBrdfPixelShader;
	if (!LoadShaderFromMemory("brdfLUT_PS", psShader, BuiltIn_BRDF_PS_ByteCode, sizeof(BuiltIn_BRDF_PS_ByteCode)))
	{
		LOG(RHILog, Error, "Failed to create brdf pixel shader");
	}
	Microsoft::WRL::ComPtr<ID3D11PixelShader> d3d11PsShader;
	psShader.shader.As(&d3d11PsShader);
	myContext->PSSetShader(d3d11PsShader.Get(), nullptr, 0);

#include "CompiledHeaders\Quad_VS.h"
	Shader vsShader = *aQuadVertexShader;
	if (!LoadShaderFromMemory("brfdLUT_VS", vsShader,  BuiltIn_Quad_VS_ByteCode, sizeof(BuiltIn_Quad_VS_ByteCode)))
	{
		LOG(RHILog, Error, "Failed to create brdf vertex shader");
	}
	Microsoft::WRL::ComPtr<ID3D11VertexShader> d3d11VsShader;
	vsShader.shader.As(&d3d11VsShader);
	myContext->VSSetShader(d3d11VsShader.Get(), nullptr, 0);

	CU::Vector2f size = { 512, 512 };
	DrawQuad(size);

	ClearRenderTargets();
	SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 127, aTexture);
	EndEvent();
	return true;
}

bool RHI::CreateBlendState(std::string_view aName, const D3D11_BLEND_DESC& aBlendDesc)
{
	ComPtr<ID3D11BlendState> blendState = {};
	const HRESULT result = myDevice->CreateBlendState(&aBlendDesc, &blendState);
	if (FAILED(result))
	{
		LOG(RHILog, Error, "Failed to create blend state {}", aName);
		return false;
	}
	SetObjectName(blendState, aName);
	std::string mapKey(aName);
	myBlendStates.emplace(mapKey, blendState);
	return true;
}

const Microsoft::WRL::ComPtr<ID3D11BlendState>& RHI::GetBlendState(const std::string& aName) const
{
	return myBlendStates.at(aName);
}

const Microsoft::WRL::ComPtr<ID3D11DepthStencilState>& RHI::GetDepthStencilState(DepthState aDepthState) const
{
	return myDepthStates[aDepthState];
}



void RHI::SetVertexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, size_t aVertexSize, size_t aVertexOffset)
{
	const unsigned vertexSize = static_cast<unsigned>(aVertexSize);
	const unsigned vertexOffset = static_cast<unsigned>(aVertexOffset);
	myContext->IASetVertexBuffers(0, 1, aVertexBuffer.GetAddressOf(), &vertexSize, &vertexOffset);
}

void RHI::SetVertexBuffers(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, const Microsoft::WRL::ComPtr<ID3D11Buffer>& anInstanceBuffer, size_t aVertexSize, size_t anInstanceSize)
{
	UINT strides[2] = { static_cast<unsigned>(aVertexSize), static_cast<unsigned>(anInstanceSize) };
	UINT offsets[2] = { 0, 0 };
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexInstanceBuffers[2] = { aVertexBuffer, anInstanceBuffer };

	myContext->IASetVertexBuffers(0, 2, vertexInstanceBuffers->GetAddressOf(), strides, offsets);
}

void RHI::SetIndexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aIndexBuffer)
{
	DXGI_FORMAT indexBufferFormat = DXGI_FORMAT_R32_UINT;
	myContext->IASetIndexBuffer(aIndexBuffer.Get(), indexBufferFormat, 0);
}

void RHI::SetConstantBuffer(const ConstantBuffer& aBuffer)
{
	if (aBuffer.myPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
	{
		myContext->VSSetConstantBuffers(aBuffer.mySlotIndex, 1, aBuffer.myBuffer.GetAddressOf());
	}
	if (aBuffer.myPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
	{
		myContext->PSSetConstantBuffers(aBuffer.mySlotIndex, 1, aBuffer.myBuffer.GetAddressOf());
	}
	if (aBuffer.myPipelineStages & PIPELINE_STAGE_GEOMETRY_SHADER)
	{
		myContext->GSSetConstantBuffers(aBuffer.mySlotIndex, 1, aBuffer.myBuffer.GetAddressOf());
	}
}

void RHI::SetPrimitiveTopology(unsigned aTopology)
{
	myContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(aTopology));
}

void RHI::SetInputLayout(const Microsoft::WRL::ComPtr<ID3D11InputLayout>& aInputLayout)
{
	myContext->IASetInputLayout(aInputLayout.Get());
}

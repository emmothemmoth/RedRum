#pragma once
#include "IAsset.h"
#include "../ViewPort.h"

#include <stdexcept>
#include <string>
#include <array>
#include <memory>
#include <wrl.h>
#include <d3d11.h>
#include <filesystem>
using namespace Microsoft::WRL;

/**
 * \brief Represents raw texture data in some format.
 */
class RHI;

class TextureAsset : public IAsset
{
	friend class RHI;

protected:

	// The name of this texture, for easy ID.
	std::string myName;
	std::string_view myNameView;

	// The actual data container for this texture.
	ComPtr<ID3D11Resource> myTexture = nullptr;

	// Different views. Which are valid and usable
	// depends on the Flags used when this texture
	// was created.
	ComPtr<ID3D11ShaderResourceView> mySRV = nullptr;


	std::array<float, 4> myClearColor = { 0,0,0,0 };

	ViewPort myViewPort = { 0, 0, 0, 0, 0, 0 };
	//D3D11_VIEWPORT myViewPort{};

	UINT myBindFlags{};
	UINT myUsageFlags{};
	UINT myAccessFlags{};
	UINT mySize;

public:
	TextureAsset() = default;
	TextureAsset(const TextureAsset& aTextureasset) = default;
	TextureAsset(const std::filesystem::path& aPath);
	~TextureAsset() = default;

	bool Load() override;
	FORCEINLINE const std::string& GetName() const { return myName; }
	FORCEINLINE UINT GetBindFlags() const { return myBindFlags; }
	FORCEINLINE UINT GetUsageFlags() const { return myUsageFlags; }
	FORCEINLINE UINT GetAccessFlags() const { return myAccessFlags; }
	FORCEINLINE bool IsValid() const { return myTexture != nullptr; }
	const Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRTV() { return myRTV; }
	const Microsoft::WRL::ComPtr<ID3D11DepthStencilView> GetDSV() { return myDSV; }
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRV() { return mySRV; }
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRV() const { return mySRV; }

	CU::Vector2f GetSize() { return { static_cast<float>(myViewPort.Width), static_cast<float>(myViewPort.Height) }; }


	CU::Vector2<unsigned> GetTextureSize() const;
	const void SetSize(CU::Vector2f aSize) { myViewPort.Width = aSize.x, myViewPort.Height = aSize.y; }

	void SetName(const std::string& aName);


public:
	// We can be either a Render Target or a Depth Stencil.
	ComPtr<ID3D11RenderTargetView> myRTV = nullptr;
	ComPtr<ID3D11DepthStencilView> myDSV = nullptr;

private:
	std::wstring myPath;

};
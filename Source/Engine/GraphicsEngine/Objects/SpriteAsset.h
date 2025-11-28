#pragma once

#include "IAsset.h"
#include "SpriteVertex.h"
//#include "MaterialAsset.h"
#include <wrl.h>

#include <filesystem>



class TextureAsset;

struct ID3D11Buffer;

enum class AnchorPoint
{
	Center,
	TopLeft,
	Top,
	TopRight,
	Right,
	BottomRight,
	Bottom,
	BottomLeft,
	Left
};

class SpriteAsset : public IAsset
{
public:
	SpriteAsset();
	SpriteAsset(const std::filesystem::path& aPath);
	~SpriteAsset();

	bool Load() override;
	void InitRelative(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, std::shared_ptr<TextureAsset> aTexture, AnchorPoint aAnchor = AnchorPoint::Center);
	void InitRelative(float aX, float aY, float aWidth, float aHeight, std::shared_ptr<TextureAsset> aTexture, AnchorPoint aAnchor = AnchorPoint::Center);

	void InitAbsolute(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, std::shared_ptr<TextureAsset> aTexture, AnchorPoint aAnchor = AnchorPoint::Center);
	void InitAbsolute(float aX, float aY, float aWidth, float aHeight, std::shared_ptr<TextureAsset> aTexture, AnchorPoint aAnchor = AnchorPoint::Center);

	void SetPositionRelative(const CU::Vector2f& aPosition);
	void SetPositionRelative(float aX, float aY);

	void SetPosition(const CU::Vector2f& aPosition);
	void SetPosition(float aX, float aY);

	void SetColor(const CU::Vector4f& aColor);
	void SetColor(const CU::Vector3f& aColor, float aAlpha);
	void SetColor(float aR, float aG, float aB, float aA);

	void SetColor(const CU::Vector3f& aColor);
	void SetColor(float aR, float aG, float aB);
	void SetAlpha(float aAlpha);

	void SetTexture(std::shared_ptr<TextureAsset> aTexture);

	CU::Vector2f GetPosition();
	CU::Vector2f GetSize();
	float GetLeft();
	float GetRight();
	float GetTop();
	float GetBottom();

	void SetAnchor(AnchorPoint aAnchor);

	void SetSourceRectangle(CU::Vector2f aStart, CU::Vector2f aSize);

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() const;
	std::shared_ptr<TextureAsset> GetTexture() const;
	const std::filesystem::path GetPath() const { return myPath; }

	void Render();

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	AnchorPoint myAnchor;
	CU::Vector2f myPosition;
	CU::Vector2f mySize;

	CU::Vector4f myColor;
	std::shared_ptr<TextureAsset> myTexture;
	std::vector<SpriteVertex> myVertices;

	std::filesystem::path myPath;
	const char* myName;
};

#include "GraphicsEngine.pch.h"

#include "GraphicsEngine.h"
#include "SpriteAsset.h"

#include "../GameEngine/MainSingleton.h"

#include "Commands/GCmdRenderSprite.h"
#include "Commands/GCmdChangePipelineState.h"
#include "Commands/GCmdSetVertexShader.h"
#include "Commands/GCmdSetPixelShader.h"

SpriteAsset::SpriteAsset() = default;

SpriteAsset::SpriteAsset(const std::filesystem::path& aPath)
{
	myPath = aPath;
}

SpriteAsset::~SpriteAsset()
{
	myVertexBuffer = nullptr;
	myTexture = nullptr;
}

bool SpriteAsset::Load()
{
	return true;
}


void SpriteAsset::InitRelative(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, std::shared_ptr<TextureAsset> aTexture, AnchorPoint aAnchor)
{
	InitRelative(aPosition.x, aPosition.y, aSize.x, aSize.y, aTexture, aAnchor);
}

void SpriteAsset::InitRelative(float aX, float aY, float aWidth, float aHeight, std::shared_ptr<TextureAsset> aTexture, AnchorPoint aAnchor)
{
	CU::Vector2<float> resolution = GetViewportSize(GraphicsEngine::Get().GetViewPort());
	InitAbsolute(aX * resolution.x, aY * resolution.y, aWidth, aHeight, aTexture, aAnchor);
}

void SpriteAsset::InitAbsolute(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, std::shared_ptr<TextureAsset> aTexture, AnchorPoint aAnchor)
{
	InitAbsolute(aPosition.x, aPosition.y, aSize.x, aSize.y, aTexture, aAnchor);
}

void SpriteAsset::InitAbsolute(float aX, float aY, float aWidth, float aHeight, std::shared_ptr<TextureAsset> aTexture, AnchorPoint aAnchor)
{
	for (int i = 0; i < 4; i++)
	{
		SpriteVertex spingus = SpriteVertex(0, 0, 0, 0);
		myVertices.push_back(spingus);
	}
	float verticalOffset = 0.0f;
	float horizontalOffset = 0.0f;

	if (aAnchor == AnchorPoint::TopLeft || aAnchor == AnchorPoint::Left || aAnchor == AnchorPoint::BottomLeft)
	{
		horizontalOffset = (aWidth * 0.5f);
	}
	if (aAnchor == AnchorPoint::TopRight || aAnchor == AnchorPoint::Right || aAnchor == AnchorPoint::BottomRight)
	{
		horizontalOffset = (aWidth * -0.5f);
	}
	if (aAnchor == AnchorPoint::TopLeft || aAnchor == AnchorPoint::Top || aAnchor == AnchorPoint::TopRight)
	{
		verticalOffset = (aHeight * 0.5f);
	}
	if (aAnchor == AnchorPoint::BottomLeft || aAnchor == AnchorPoint::Bottom || aAnchor == AnchorPoint::BottomRight)
	{
		verticalOffset = (aHeight * -0.5f);
	}

	myVertices[0].position = { (aX - (aWidth * 0.5f)) + horizontalOffset, (aY - (aHeight * 0.5f)) + verticalOffset, 1, 1 }; //bottom left
	myVertices[1].position = { (aX + (aWidth * 0.5f)) + horizontalOffset, (aY - (aHeight * 0.5f)) + verticalOffset, 1, 1 }; //bottom right
	myVertices[2].position = { (aX - (aWidth * 0.5f)) + horizontalOffset, (aY + (aHeight * 0.5f)) + verticalOffset, 1, 1 }; //top left
	myVertices[3].position = { (aX + (aWidth * 0.5f)) + horizontalOffset, (aY + (aHeight * 0.5f)) + verticalOffset, 1, 1 }; //top right
	myVertices[0].uv = { 0, 0 }; //bottom left
	myVertices[1].uv = { 1, 0 }; //bottom right
	myVertices[2].uv = { 0, 1 }; //top left
	myVertices[3].uv = { 1, 1 }; //top right
	myTexture = aTexture;
	myVertexBuffer.Reset();
	GraphicsEngine::Get().CreateVertexBuffer("interface vertex buffer", myVertices, myVertexBuffer);

	myColor = { 1.0f ,1.0f ,1.0f ,1.0f };

	myPosition.x = aX;
	myPosition.y = aY;

	mySize.x = aWidth;
	mySize.y = aHeight;
}

void SpriteAsset::SetPositionRelative(const CU::Vector2f& aPosition)
{
	SetPositionRelative(aPosition.x, aPosition.y);
}

void SpriteAsset::SetPositionRelative(float aX, float aY)
{
	CU::Vector2<float> resolution = GetViewportSize(GraphicsEngine::Get().GetViewPort());
	SetPosition(aX * resolution.x, aY * resolution.y);
}

void SpriteAsset::SetPosition(const CU::Vector2f& aPosition)
{
	SetPosition(aPosition.x, aPosition.y);
}

void SpriteAsset::SetPosition(float aX, float aY)
{
	float verticalOffset = 0.0f;
	float horizontalOffset = 0.0f;

	if (myAnchor == AnchorPoint::TopLeft || myAnchor == AnchorPoint::Left || myAnchor == AnchorPoint::BottomLeft)
	{
		horizontalOffset = (mySize.x * 0.5f);
	}
	if (myAnchor == AnchorPoint::TopRight || myAnchor == AnchorPoint::Right || myAnchor == AnchorPoint::BottomRight)
	{
		horizontalOffset = (mySize.x * -0.5f);
	}
	if (myAnchor == AnchorPoint::TopLeft || myAnchor == AnchorPoint::Top || myAnchor == AnchorPoint::TopRight)
	{
		verticalOffset = (mySize.y * 0.5f);
	}
	if (myAnchor == AnchorPoint::BottomLeft || myAnchor == AnchorPoint::Bottom || myAnchor == AnchorPoint::BottomRight)
	{
		verticalOffset = (mySize.y * -0.5f);
	}

	myVertices[0].position = { (aX - (mySize.x * 0.5f)) + horizontalOffset, (aY - (mySize.y * 0.5f)) + verticalOffset, 1, 1 }; //bottom left
	myVertices[1].position = { (aX + (mySize.x * 0.5f)) + horizontalOffset, (aY - (mySize.y * 0.5f)) + verticalOffset, 1, 1 }; //bottom right
	myVertices[2].position = { (aX - (mySize.x * 0.5f)) + horizontalOffset, (aY + (mySize.y * 0.5f)) + verticalOffset, 1, 1 }; //top left
	myVertices[3].position = { (aX + (mySize.x * 0.5f)) + horizontalOffset, (aY + (mySize.y * 0.5f)) + verticalOffset, 1, 1 }; //top right

	myVertexBuffer.Reset();
	GraphicsEngine::Get().CreateVertexBuffer("interface vertex buffer", myVertices, myVertexBuffer);

	myPosition.x = aX;
	myPosition.y = aY;
}

void SpriteAsset::SetColor(const CU::Vector4f& aColor)
{
	SetColor(aColor.x, aColor.y, aColor.z, aColor.w);
}

void SpriteAsset::SetColor(const CU::Vector3f& aColor, float aAlpha)
{
	SetColor(aColor.x, aColor.y, aColor.z, aAlpha);
}

void SpriteAsset::SetColor(float aR, float aG, float aB, float aA)
{
	myVertices[0].tint = { aR, aG, aB, aA };
	myVertices[1].tint = { aR, aG, aB, aA };
	myVertices[2].tint = { aR, aG, aB, aA };
	myVertices[3].tint = { aR, aG, aB, aA };
	myColor = { aR, aG, aB, aA };
	myVertexBuffer.Reset();
	GraphicsEngine::Get().CreateVertexBuffer("interface vertex buffer", myVertices, myVertexBuffer);
}

void SpriteAsset::SetColor(const CU::Vector3f& aColor)
{
	SetColor(aColor.x, aColor.y, aColor.z);
}

void SpriteAsset::SetColor(float aR, float aG, float aB)
{
	myColor = { aR, aG, aB, myColor.w };
	SetColor(myColor);
}

void SpriteAsset::SetAlpha(float aAlpha)
{
	myColor.w = aAlpha;
	SetColor(myColor);
}

void SpriteAsset::SetTexture(std::shared_ptr<TextureAsset> aTexture)
{
	myTexture = aTexture;
}

CU::Vector2f SpriteAsset::GetPosition()
{
	return myPosition;
}

CU::Vector2f SpriteAsset::GetSize()
{
	return mySize;
}

float SpriteAsset::GetLeft()
{
	float left;
	if (myAnchor == AnchorPoint::TopLeft || myAnchor == AnchorPoint::Left || myAnchor == AnchorPoint::BottomLeft) //Offset here should maybe be 0, will see with testing
	{
		left = myPosition.x;
	}
	else if (myAnchor == AnchorPoint::TopRight || myAnchor == AnchorPoint::Right || myAnchor == AnchorPoint::BottomRight)
	{
		left = myPosition.x - mySize.x * 1.0f;
	}
	else
	{
		left = myPosition.x - mySize.x * 0.5f;
	}
	return left;
}

float SpriteAsset::GetRight()
{
	float right;
	if (myAnchor == AnchorPoint::TopLeft || myAnchor == AnchorPoint::Left || myAnchor == AnchorPoint::BottomLeft) //Offset here should maybe be 0, will see with testing
	{
		right = myPosition.x + mySize.x * 1.0f;
	}
	else if (myAnchor == AnchorPoint::TopRight || myAnchor == AnchorPoint::Right || myAnchor == AnchorPoint::BottomRight)
	{
		right = myPosition.x;
	}
	else
	{
		right = myPosition.x + mySize.x * 0.5f;
	}
	return right;
}

float SpriteAsset::GetTop()
{
	float top;
	if (myAnchor == AnchorPoint::TopLeft || myAnchor == AnchorPoint::Top || myAnchor == AnchorPoint::TopRight) //XD lmao
	{
		top = myPosition.y;
	}
	else if (myAnchor == AnchorPoint::BottomLeft || myAnchor == AnchorPoint::Bottom || myAnchor == AnchorPoint::BottomRight)
	{
		top = myPosition.y - mySize.y * 1.0f;
	}
	else
	{
		top = myPosition.y - mySize.y * 0.5f;
	}
	return top;
}

float SpriteAsset::GetBottom()
{
	float bottom;
	if (myAnchor == AnchorPoint::TopLeft || myAnchor == AnchorPoint::Top || myAnchor == AnchorPoint::TopRight)
	{
		bottom = myPosition.y + mySize.y * 1.0f;
	}
	else if (myAnchor == AnchorPoint::BottomLeft || myAnchor == AnchorPoint::Bottom || myAnchor == AnchorPoint::BottomRight)
	{
		bottom = myPosition.y;
	}
	else
	{
		bottom = myPosition.y + mySize.y * 0.5f;
	}
	return bottom;
}

void SpriteAsset::SetAnchor(AnchorPoint aAnchor)
{
	myAnchor = aAnchor;
	SetPosition(myPosition);
}

void SpriteAsset::SetSourceRectangle(CU::Vector2f aStart, CU::Vector2f aSize)
{
	CU::Vector2<unsigned> textureSize = myTexture->GetTextureSize();
	float horizontal = aStart.x / static_cast<float>(textureSize.x); //displacement in UV start compared to texture size
	float vertical = aStart.y / static_cast<float>(textureSize.y);

	float horizontalSize = aSize.x / static_cast<float>(textureSize.x);
	float verticalSize = aSize.y / static_cast<float>(textureSize.y);

	myVertices[0].uv = { 0 + horizontal,		0 + vertical }; //bottom left
	myVertices[1].uv = { horizontal + horizontalSize,	0 + vertical }; //bottom right
	myVertices[2].uv = { 0 + horizontal,		vertical + verticalSize }; //top left
	myVertices[3].uv = { horizontal + horizontalSize,	vertical + verticalSize }; //top right
	myVertexBuffer.Reset();
	GraphicsEngine::Get().CreateVertexBuffer("interface vertex buffer", myVertices, myVertexBuffer);
}

Microsoft::WRL::ComPtr<ID3D11Buffer> SpriteAsset::GetVertexBuffer() const
{
	return myVertexBuffer;
}

std::shared_ptr<TextureAsset> SpriteAsset::GetTexture() const
{
	return myTexture;
}

void SpriteAsset::Render()
{
	auto& renderer = RENDERER;
	renderer.Enqueue<GCmdSetVertexShader>("UI_VS");
	renderer.Enqueue<GCmdSetPixelShader>("UI_PS");
	renderer.Enqueue<GCmdRenderSprite>(this);
}


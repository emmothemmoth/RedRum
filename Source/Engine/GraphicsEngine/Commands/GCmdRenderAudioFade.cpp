#include "GraphicsEngine.pch.h"
#include "GCmdRenderAudioFade.h"

#include "../Objects/TextureAsset.h"
#include "GraphicsEngine.h"
#include <Objects/SpriteVertex.h>


GCmdRenderAudioFade::GCmdRenderAudioFade(std::shared_ptr<TextureAsset> aRenderTarget, const float aFadeinTime, const float aFadeOutTime, const float aTotalTime, const float aRegionStart, const float aRegionEnd)
{
	myRenderTarget = aRenderTarget;
	myFadeInTime = aFadeinTime;
	myFadeOutTime = aFadeOutTime;
	myTotalTime = aTotalTime;
	myRegionStart = aRegionStart;
	myRegionEnd = aRegionEnd;
}

void GCmdRenderAudioFade::Execute()
{
	GraphicsEngine::Get().ChangeRenderTarget(myRenderTarget);
	GraphicsEngine::Get().ClearRenderTarget(myRenderTarget);
	std::vector<SpriteVertex> leftVertices;
	std::vector<unsigned> leftIndices;
	std::vector<SpriteVertex> rightVertices;
	std::vector<unsigned> rightIndices;

	//Somethings is not working -> cannot just use regionstart * size!!
	float xMin = myRegionStart * static_cast<float>(myRenderTarget->GetTextureSize().x);
	float xMax = myRegionEnd * static_cast<float>(myRenderTarget->GetTextureSize().x);
	float yMin = 0.0f;
	float yMax = static_cast<float>(myRenderTarget->GetTextureSize().y);

	float fadeInX = xMin + ((myFadeInTime / myTotalTime) * (xMax - xMin));
	float fadeOutX = xMax - ((myFadeOutTime / myTotalTime) * (xMax-xMin));

	leftVertices.emplace_back(xMin, yMax, 0.0f, 0.0f);
	leftIndices.emplace_back(0);

	leftVertices.emplace_back(fadeInX, yMin, 0.0f, 0.0f);
	leftIndices.emplace_back(1);

	{

		ComPtr<ID3D11Buffer> indexBuffer;
		GraphicsEngine::Get().CreateIndexBuffer("Audio index buffer", leftIndices, indexBuffer);
		GraphicsEngine::Get().SetIndexBuffer(indexBuffer);

		ComPtr<ID3D11Buffer> vertexBuffer;
		GraphicsEngine::Get().CreateVertexBuffer("Audio vertex buffer", leftVertices, vertexBuffer);
		GraphicsEngine::Get().SetVertexBuffer(vertexBuffer);

		GraphicsEngine::Get().RenderAudioUI(0, static_cast<unsigned>(leftIndices.size()));
	}

	rightVertices.emplace_back(fadeOutX, yMin, 0, 0);
	rightIndices.emplace_back(0);

	rightVertices.emplace_back(xMax, yMax, 0, 0);
	rightIndices.emplace_back(1);

	{

		ComPtr<ID3D11Buffer> indexBuffer;
		GraphicsEngine::Get().CreateIndexBuffer("Audio right index buffer", rightIndices, indexBuffer);
		GraphicsEngine::Get().SetIndexBuffer(indexBuffer);

		ComPtr<ID3D11Buffer> vertexBuffer;
		GraphicsEngine::Get().CreateVertexBuffer("Audio right vertex buffer", rightVertices, vertexBuffer);
		GraphicsEngine::Get().SetVertexBuffer(vertexBuffer);

		GraphicsEngine::Get().RenderAudioUI(0, static_cast<unsigned>(rightIndices.size()));
	}

	GraphicsEngine::Get().ChangeRenderTarget(GraphicsEngine::Get().GetBackBuffer());

}

void GCmdRenderAudioFade::Destroy()
{
}

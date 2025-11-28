#include "GraphicsEngine.pch.h"
#include "GCmdRenderAudioWave.h"

#include "../Objects/TextureAsset.h"
#include "GraphicsEngine.h"
#include <Objects/SpriteVertex.h>



GCmdRenderAudioWave::GCmdRenderAudioWave(std::shared_ptr<TextureAsset> aRenderTarget, float* someAudioData, unsigned aSizeOfAudioData)
{
	//TODO: Get info about amount of channels
	myRenderTarget = aRenderTarget;
	myAudioData = someAudioData;
	myAudioSize = aSizeOfAudioData;
}


void GCmdRenderAudioWave::Execute()
{
	std::vector<double> audio;
	for (unsigned index = 0; index < myAudioSize; index++)
	{
		audio.push_back(static_cast<double>(static_cast<double>(myAudioData[index])));
	}
	GraphicsEngine::Get().ChangeRenderTarget(myRenderTarget);
	std::vector<SpriteVertex> leftVertices;
	std::vector<unsigned> leftIndices;
	std::vector<SpriteVertex> rightVertices;
	std::vector<unsigned> rightIndices;

	//unsigned int textureX = myRenderTarget->GetTextureSize().x;
	float stepSize = static_cast<float>(myRenderTarget->GetTextureSize().x) / (static_cast<float>(audio.size()) * 0.5f);
	float rightChannelY = static_cast<float>(myRenderTarget->GetTextureSize().y) * 0.5f;

	//left channel
	for (int index = 0; index < audio.size()-1; index+= 2)
	{
		float x = (index / 2) * stepSize;
		float y = (((static_cast<float>(audio[index]) + 1.0f) * 0.5f) * static_cast<float>(myRenderTarget->GetTextureSize().y / 2));
		leftVertices.emplace_back(x, y, 0.0f, 0.0f);
			leftIndices.push_back(index / 2);
	}
	{

		ComPtr<ID3D11Buffer> indexBuffer;
		GraphicsEngine::Get().CreateIndexBuffer("Audio index buffer", leftIndices, indexBuffer);
		GraphicsEngine::Get().SetIndexBuffer(indexBuffer);

		ComPtr<ID3D11Buffer> vertexBuffer;
		GraphicsEngine::Get().CreateVertexBuffer("Audio vertex buffer", leftVertices, vertexBuffer);
		GraphicsEngine::Get().SetVertexBuffer(vertexBuffer);

		GraphicsEngine::Get().RenderAudioUI(0, static_cast<unsigned>(leftIndices.size()));
	}

	//right channel
	for (int index = 1; index < audio.size(); index += 2)
	{
		float x = (index / 2)*stepSize;
		float y = rightChannelY + (((static_cast<float>(audio[index]) + 1.0f) * 0.5f) * static_cast<float>(myRenderTarget->GetTextureSize().y / 2));
		rightVertices.emplace_back(x, y, 0.0f, 0.0f);
			rightIndices.push_back(index/2);
	}

	{

		ComPtr<ID3D11Buffer> indexBuffer;
		GraphicsEngine::Get().CreateIndexBuffer("Audio right index buffer", rightIndices, indexBuffer);
		GraphicsEngine::Get().SetIndexBuffer(indexBuffer);

		ComPtr<ID3D11Buffer> vertexBuffer;
		GraphicsEngine::Get().CreateVertexBuffer("Audio right vertex buffer", rightVertices, vertexBuffer);
		GraphicsEngine::Get().SetVertexBuffer(vertexBuffer);

		GraphicsEngine::Get().RenderAudioUI(0, static_cast<unsigned>(rightIndices.size()));
	}
}

void GCmdRenderAudioWave::Destroy()
{
	myRenderTarget = nullptr;
}

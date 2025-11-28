#pragma once

#include "GraphicsCommandList.h"

class TextureAsset;

//Note: This is part of initialization. Should not be done every frame!
class GCmdRenderAudioWave : public GraphicsCommandBase
{
public:
	GCmdRenderAudioWave(std::shared_ptr<TextureAsset> aRenderTarget, float* someAudioData, unsigned aSizeOfAudioData);
	~GCmdRenderAudioWave() override = default;

	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<TextureAsset> myRenderTarget;
	float* myAudioData;
	unsigned myAudioSize;
};

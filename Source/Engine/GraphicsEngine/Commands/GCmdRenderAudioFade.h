#pragma once


#include "GraphicsCommandList.h"

class TextureAsset;

//Note: This is part of initialization. Should not be done every frame!

class GCmdRenderAudioFade : public GraphicsCommandBase
{
public:
	GCmdRenderAudioFade(std::shared_ptr<TextureAsset> aRenderTarget, const float aFadeinTime, const float aFadeOutTime, const float aTotalTime, const float aRegionStart, const float aRegionEnd);
	~GCmdRenderAudioFade() override = default;

	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<TextureAsset> myRenderTarget;
	float myFadeInTime = 0.0f;
	float myFadeOutTime = 0.0f;
	float myTotalTime = 0.0f;
	float myRegionStart = 0.0f;
	float myRegionEnd = 1.0f;
};

#include "GraphicsEngine.pch.h"
#include "GCmdRenderSprite.h"

#include "GraphicsEngine.h"

#include "../Objects/SpriteAsset.h"

GCmdRenderSprite::GCmdRenderSprite(SpriteAsset* aElement)
{
	myElement = aElement;
}

GCmdRenderSprite::~GCmdRenderSprite()
{
}

void GCmdRenderSprite::Execute()
{
	GraphicsEngine::Get().RenderSprite2D(*myElement);
}

void GCmdRenderSprite::Destroy()
{
	myElement = nullptr;
}

#include "GraphicsEngine.pch.h"
#include "GCmdChangeRenderTarget.h"

#include "../Objects/TextureAsset.h"
#include "../GraphicsEngine.h"

GCmdChangeRenderTarget::GCmdChangeRenderTarget(std::shared_ptr<TextureAsset> aRenderTarget)
{
	myRenderTarget = aRenderTarget;
}

void GCmdChangeRenderTarget::Execute()
{
	GraphicsEngine::Get().ChangeRenderTarget(myRenderTarget);
}

void GCmdChangeRenderTarget::Destroy()
{
	myRenderTarget = nullptr;
}

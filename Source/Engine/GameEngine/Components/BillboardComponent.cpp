#include "GameEngine.pch.h"
#include "BillboardComponent.h"
#include "GameObject.h"
#include "MainSingleton.h"
#include "../../GraphicsEngine/Objects/TextureAsset.h"
#include "../../GraphicsEngine/Commands/GCmdDrawBillboard.h"

BillboardComponent::BillboardComponent(GameObject& aParent)
	: Component(aParent)
{
	myComponentType = ComponentType::Billboard;
	myRenderStages.at(RenderStage::ShadowMapping) = false;
	myRenderStages.at(RenderStage::Deferred) = false;
	myRenderStages.at(RenderStage::WorldSpaceUI) = true;
	myIsVisible = false;
}

BillboardComponent::~BillboardComponent()
{
	myTexture = nullptr;
}

void BillboardComponent::SetTexture(std::shared_ptr<TextureAsset> aTexture)
{
	if (!aTexture) return;
	myTexture = aTexture;
}

void BillboardComponent::SetOffset(const CU::Vector4f& anOffset)
{
	myOffset = anOffset;
}

void BillboardComponent::Render()
{
	if (!myIsVisible) return;
	auto& renderer = RENDERER;
	CU::Matrix4x4f transform = myParent.GetTransform();
	transform.SetRow(transform.GetRow(4) + myOffset, 4);
	renderer.Enqueue<GCmdDrawBillboard>(RenderStage::WorldSpaceUI, myTexture, transform);
}

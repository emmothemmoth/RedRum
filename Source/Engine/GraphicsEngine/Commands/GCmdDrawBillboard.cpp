#include "GraphicsEngine.pch.h"
#include "GCmdDrawBillboard.h"
#include "GraphicsEngine.h"
#include "../Objects/TextureAsset.h"
#include "../Buffers/ObjectBuffer.h"

GCmdDrawBillboard::GCmdDrawBillboard(std::shared_ptr<TextureAsset> aTexture, const CU::Matrix4x4f& aTransform)
{
	myTexture = aTexture;
	myTransform = aTransform;
}

GCmdDrawBillboard::~GCmdDrawBillboard()
{
	myTexture = nullptr;
}

void GCmdDrawBillboard::Execute()
{
	auto& engine = GraphicsEngine::Get();
	ObjectBufferData objectBuffer;
	objectBuffer.Transform = myTransform;
	objectBuffer.InverseTranspose = CU::Matrix4x4f::Transpose(myTransform.GetInverse());
	engine.UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objectBuffer);
	engine.RenderBillboard(myTexture);
}

void GCmdDrawBillboard::Destroy()
{
	myTexture = nullptr;
}

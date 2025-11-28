#include "GraphicsEngine.pch.h"
#include "GCmdDrawDebugLines.h"
#include "../Objects/MeshAsset.h"
#include "../Objects/DebugLineObject.h"

#include "../Buffers/ObjectBuffer.h"

#include "../GraphicsEngine.h"

GCmdDrawDebugLines::GCmdDrawDebugLines(std::shared_ptr<DebugLineObject> someDebugLines, const CU::Matrix4x4f& aTransform)
{
	myDebugName = someDebugLines->GetName();
	myDebugObject = someDebugLines;
	myTransform = aTransform;
}


GCmdDrawDebugLines::~GCmdDrawDebugLines() = default;

void GCmdDrawDebugLines::Execute()
{
	ObjectBufferData objectBufferData;
	objectBufferData.Transform = myTransform;
	objectBufferData.InverseTranspose = CU::Matrix4x4<float>::Transpose(myTransform.GetInverse());
	objectBufferData.HasBone = myHasBones;

	auto& engine = GraphicsEngine::Get();
	engine.UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objectBufferData);
	//engine.ChangeRenderTarget(GraphicsEngine::Get().GetLDRBuffer());
	//engine.SetVertexShader("Default_VS");
	//engine.SetPixelShader("DebugLine_PS");
	engine.RenderDebugLines(*myDebugObject);

}

void GCmdDrawDebugLines::Destroy()
{
	myDebugObject = nullptr;
	myDebugName.~basic_string();
}

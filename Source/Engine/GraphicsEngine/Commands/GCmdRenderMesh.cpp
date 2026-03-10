#include "InterOp/RHI.h"

#include "GraphicsEngine.pch.h"
#include "GCmdRenderMesh.h"
#include "../GraphicsEngine.h"

#include "..\Objects\MeshAsset.h"
#include "../Objects/MaterialAsset.h"

#include "Buffers\ObjectBuffer.h"
#include "Buffers\ObjectIDBuffer.h"


GCmdRenderMesh::GCmdRenderMesh(std::shared_ptr<MeshAsset> aMesh, CU::Matrix4x4f aTransform, std::vector<std::shared_ptr<MaterialAsset>> aMaterialList, const unsigned anObjectID)
{
	myMesh = aMesh;
	myTransform = aTransform;
	myMaterialList = aMaterialList;
	myObjectID = anObjectID;
}

void GCmdRenderMesh::Execute()
{
	ObjectBufferData objectBuffer;
	objectBuffer.Transform = myTransform;
	objectBuffer.InverseTranspose = CU::Matrix4x4f::Transpose(myTransform.GetInverse());
	objectBuffer.HasBone =  !myMesh->GetSkeleton().bones.empty();
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objectBuffer);

	ObjectIDBufferData IDBuffer;

	for (auto& element : myMesh->GetElements())
	{
		IDBuffer.ObjectID = EncodeID(element.PartID);
		GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectIDBuffer, IDBuffer);
		GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer,
			myMaterialList[element.MaterialIndex]->GetMaterialBuffer());
	}

	GraphicsEngine::Get().RenderMesh(*myMesh, myMaterialList);
	
}

void GCmdRenderMesh::Destroy()
{
	myMesh = nullptr;
	for (auto& material : myMaterialList)
	{
		material = nullptr;
	}
	myMaterialList.clear();
}

uint32_t GCmdRenderMesh::EncodeID(uint8_t aPartID)
{
	return (myObjectID << 8) | aPartID;
}

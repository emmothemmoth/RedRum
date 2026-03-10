#include "GraphicsEngine.pch.h"
#include "GCmdRenderInstancedMesh.h"

#include "Objects\MeshAsset.h"
#include "Objects\InstanceData.h"
#include "Objects\SkeletonAsset.h"
#include "../Buffers/ObjectBuffer.h"

#include "GraphicsEngine.h"
#include <Buffers/ObjectIDBuffer.h>

GCmdRenderInstancedMesh::GCmdRenderInstancedMesh(const std::shared_ptr<MeshAsset> aMesh, const CU::Matrix4x4f& anObjectTransform, const std::shared_ptr<InstanceData> someInstanceData, std::vector<std::shared_ptr<MaterialAsset>> aMaterialList, const unsigned anObjectID)
{
	myMaterialList = aMaterialList;
	myMesh = aMesh;
	myInstanceData = someInstanceData;
	myTransform = anObjectTransform;
	myObjectID = anObjectID;
}

void GCmdRenderInstancedMesh::Execute()
{
	ObjectBufferData objectBuffer;
	objectBuffer.Transform = myTransform;
	objectBuffer.HasBone = false;
	objectBuffer.IsInstanced = true;
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objectBuffer);

	ObjectIDBufferData IDBuffer;
	IDBuffer.ObjectID = myObjectID; 
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectIDBuffer, IDBuffer);

	if (myMaterialList.empty())
	{
		GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, GraphicsEngine::Get().GetDefaultMaterials());
	}
	else
	{
		GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, myMaterialList);
	}
	VertexBuffer buffer;
	buffer.Buffer = myInstanceData->GetInstanceBuffer();
	buffer.NumVertices = myInstanceData->GetRelativeTransforms().size();
	buffer.VertexStride = sizeof(CU::Matrix4x4f);
	buffer.DataSize = buffer.VertexStride * buffer.NumVertices;
	GraphicsEngine::Get().UpdateVertexBuffer(buffer, myInstanceData->GetRelativeTransforms());
	GraphicsEngine::Get().RenderInstancedMesh(*myMesh, myMaterialList, *myInstanceData);
}

void GCmdRenderInstancedMesh::Destroy()
{
	myMesh = nullptr;
	myInstanceData = nullptr;
	for (auto& material : myMaterialList)
	{
		material = nullptr;
	}
	myMaterialList.clear();
}

uint32_t GCmdRenderInstancedMesh::EncodeID(uint8_t aPartID)
{
	return (myObjectID << 8) | aPartID;
}

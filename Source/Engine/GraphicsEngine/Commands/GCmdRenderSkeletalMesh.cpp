
#include "GraphicsEngine.pch.h"
#include "GCmdRenderSkeletalMesh.h"

#include "../Objects/MaterialAsset.h"
#include "GraphicsEngine.h"

#include "Buffers\ObjectBuffer.h"
#include <Buffers/ObjectIDBuffer.h>

GCmdRenderSkeletalMesh::GCmdRenderSkeletalMesh(const std::shared_ptr<MeshAsset> aMesh, CU::Matrix4x4f aTransform,  const std::vector<CommonUtilities::Matrix4x4<float>> aBoneTransformList, std::vector<std::shared_ptr<MaterialAsset>> aMaterialList, unsigned anID)
{
	myMesh = aMesh;
	myTransform = aTransform;
	myBoneTransforms.resize(aBoneTransformList.size());
	myBoneTransforms = aBoneTransformList;
	myMaterialList = aMaterialList;
	myObjectID = anID;

}

void GCmdRenderSkeletalMesh::Execute()
{
	ObjectBufferData objectBuffer;
	objectBuffer.Transform = myTransform;
	objectBuffer.HasBone = true;
	for (int index = 0; index < myBoneTransforms.size(); ++index)
	{
		objectBuffer.BoneTransforms[index] = myBoneTransforms[index];
	}
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

void GCmdRenderSkeletalMesh::Destroy()
{
	myMesh = nullptr;
	for (auto& material : myMaterialList)
	{
		material = nullptr;
	}
	myMaterialList.clear();
}

uint32_t GCmdRenderSkeletalMesh::EncodeID(uint8_t aPartID)
{
	return (myObjectID << 8) | aPartID;
}
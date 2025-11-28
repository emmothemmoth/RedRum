#include "AssetManager.pch.h"
#include "MeshAssetHandler.h"
#include "CommonUtilities\RandomWrapper.h"
#include "CommonUtilities\UtilityFunctions.hpp"
#include "Primitives.h"

#include "TGAFbx.h"
#include <corecrt_math_defines.h>

void MeshAssetHandler::InitGround(MeshAsset& inOutAsset)
{
	GroundData ground;
	inOutAsset.AddElement(ground.mdlVertices, ground.mdlIndices, 0);
}

void MeshAssetHandler::InitCube(MeshAsset& inOutAsset)
{
	CubeData data;
	inOutAsset.AddElement(data.mdlVertices, data.mdlIndices, 0);
}

void MeshAssetHandler::InitSphere(MeshAsset& inOutAsset)
{
	const float radius = 50.0f;
	const unsigned int sectors = 36;
	const unsigned int stacks = 18;

	std::vector<Vertex> mdlVertices;
	std::vector<unsigned> mdlIndices;

	float sectorStep = 2.0f * static_cast<float>(M_PI) / sectors;
	float stackStep = static_cast<float>(M_PI) / stacks;
	float sectorAngle, stackAngle;

	for (unsigned int i = 0; i <= stacks; ++i)
	{
		stackAngle = static_cast<float>(M_PI) / 2.0f - static_cast<float>(i * stackStep);
		float xyOuter = radius * cosf(stackAngle);
		float zOuter = radius * sinf(stackAngle);

		for (unsigned int j = 0; j <= sectors; ++j)
		{
			sectorAngle = j * sectorStep;

			float x = xyOuter * cosf(sectorAngle);
			float y = xyOuter * sinf(sectorAngle);
			float z = zOuter;

			CommonUtilities::Vector3<float> normal(x, y, z);
			normal.Normalize();


			CommonUtilities::Vector3<float> tangent(1.0f, 0.0f, 0.0f);

			float U = static_cast<float>(j) / sectors;
			float V = static_cast<float>(i) / stacks;


			CommonUtilities::Vector3<float> position{ x, y, z };
			CommonUtilities::Vector2<float> uv{ U, V };
			Vertex vertex(position, uv, normal, tangent);

			mdlVertices.push_back(vertex);


		}
	}

	unsigned int k1, k2;
	for (unsigned int i = 0; i < stacks; ++i)
	{
		k1 = i * (sectors + 1);
		k2 = k1 + sectors + 1;

		for (unsigned int j = 0; j < sectors; ++j, ++k1, ++k2)
		{
			if (i != 0)
			{
				mdlIndices.push_back(k1);
				mdlIndices.push_back(k2);
				mdlIndices.push_back(k1 + 1);
				//// Triangle 1
				//if (isInverted)
				//{
				//	sphereIndices.push_back(k2);
				//	sphereIndices.push_back(k1);
				//	sphereIndices.push_back(k1 + 1);
				//}
				//else
				//{
				//	sphereIndices.push_back(k1);
				//	sphereIndices.push_back(k2);
				//	sphereIndices.push_back(k1 + 1);
				//}
			}
			if (i != (stacks - 1))
			{
				mdlIndices.push_back(k1 + 1);
				mdlIndices.push_back(k2);
				mdlIndices.push_back(k2 + 1);
				// Triangle 2
				//if (isInverted)
				//{
				//	sphereIndices.push_back(k2 + 1);
				//	sphereIndices.push_back(k2);
				//	sphereIndices.push_back(k1 + 1);
				//}
				//else
				//{
				//	sphereIndices.push_back(k1 + 1);
				//	sphereIndices.push_back(k2);
				//	sphereIndices.push_back(k2 + 1);
				//}
			}
		}
	}

	//Element element;
	//element.VertexOffset = 0;
	//element.IndexOffset = 0;
	//element.NumVertices = static_cast<unsigned>(mdlVertices.size());
	//element.NumIndices = static_cast<unsigned>(mdlIndices.size());


//SphereData sphere;
	inOutAsset.AddElement(mdlVertices, mdlIndices, 0);
}

void MeshAssetHandler::InitHorizontalPlane(MeshAsset& inOutAsset)
{
	HorizontalPlaneData plane;
	inOutAsset.AddElement(plane.mdlVertices, plane.mdlIndices, 0);
}

void MeshAssetHandler::InitVerticalPlane(MeshAsset& inOutAsset)
{
	VerticalPlaneData plane;
	inOutAsset.AddElement(plane.mdlVertices, plane.mdlIndices, 0);
}

bool MeshAssetHandler::LoadMeshFromFBX(const std::filesystem::path& aPath, MeshAsset& inOutAsset)
{
	TGA::FBX::Importer::InitImporter();
	if (aPath == L"CubeMesh")
	{
		InitCube(inOutAsset);
		return true;
	}
	else if (aPath == L"GroundMesh")
	{
		InitGround(inOutAsset);
		return true;
	}
	else if (aPath == L"SphereMesh")
	{
		InitSphere(inOutAsset);
		return true;
	}
	else if (aPath == L"HorizontalPlaneMesh")
	{
		InitHorizontalPlane(inOutAsset);
		return true;
	}
	else if (aPath == L"VerticalPlaneMesh")
	{
		InitVerticalPlane(inOutAsset);
		return true;
	}
	std::wstring contentPath = L"Content\\" + aPath.wstring();

	TGA::FBX::Mesh tgaMesh;
	if (TGA::FBX::Importer::LoadMeshW(contentPath, tgaMesh))
	{
		bool hasBone = false;
		if (tgaMesh.Skeleton.Bones.size() > 0)
		{
			hasBone = true;
		}
		//MeshAsset mesh;
		for (int index = 0; index < tgaMesh.Elements.size(); ++index)
		{
			std::vector<Vertex> vertices;
			for (int vertexIndex = 0; vertexIndex < tgaMesh.Elements[index].Vertices.size(); ++vertexIndex)
			{
				float x = tgaMesh.Elements[index].Vertices[vertexIndex].Position[0];
				float y = tgaMesh.Elements[index].Vertices[vertexIndex].Position[1];
				float z = tgaMesh.Elements[index].Vertices[vertexIndex].Position[2];
				//CommonUtilities::RandomWrapper random;
				//float r = static_cast<float>(random.GetRandomInt(0, 255) / 255.0f);
				//float g = static_cast<float>(random.GetRandomInt(0, 255) / 255.0f);
				//float b = static_cast<float>(random.GetRandomInt(0, 255) / 255.0f);
				//float a = 1.0f;

				float normalX = tgaMesh.Elements[index].Vertices[vertexIndex].Normal[0];
				float normalY = tgaMesh.Elements[index].Vertices[vertexIndex].Normal[1];
				float normalZ = tgaMesh.Elements[index].Vertices[vertexIndex].Normal[2];

				float tangentX = tgaMesh.Elements[index].Vertices[vertexIndex].Tangent[0];
				float tangentY = tgaMesh.Elements[index].Vertices[vertexIndex].Tangent[1];
				float tangentZ = tgaMesh.Elements[index].Vertices[vertexIndex].Tangent[2];

				Vertex vertex = { x, y, z, normalX, normalY, normalZ, tangentX, tangentY, tangentZ };
				vertex.myUV.x = tgaMesh.Elements[index].Vertices[vertexIndex].UVs[0][0];
				vertex.myUV.y = tgaMesh.Elements[index].Vertices[vertexIndex].UVs[0][1];
				vertex.Tangent.x = tgaMesh.Elements[index].Vertices[vertexIndex].Tangent[0];
				vertex.Tangent.y = tgaMesh.Elements[index].Vertices[vertexIndex].Tangent[1];
				vertex.Tangent.z = tgaMesh.Elements[index].Vertices[vertexIndex].Tangent[2];
				vertex.BoneIDs.x = tgaMesh.Elements[index].Vertices[vertexIndex].BoneIDs[0];
				vertex.BoneIDs.y = tgaMesh.Elements[index].Vertices[vertexIndex].BoneIDs[1];
				vertex.BoneIDs.z = tgaMesh.Elements[index].Vertices[vertexIndex].BoneIDs[2];
				vertex.BoneIDs.w = tgaMesh.Elements[index].Vertices[vertexIndex].BoneIDs[3];
				vertex.BoneWeights.x = tgaMesh.Elements[index].Vertices[vertexIndex].BoneWeights[0];
				vertex.BoneWeights.y = tgaMesh.Elements[index].Vertices[vertexIndex].BoneWeights[1];
				vertex.BoneWeights.z = tgaMesh.Elements[index].Vertices[vertexIndex].BoneWeights[2];
				vertex.BoneWeights.w = tgaMesh.Elements[index].Vertices[vertexIndex].BoneWeights[3];
				vertices.push_back(vertex);
			}

			inOutAsset.AddElement(vertices, tgaMesh.Elements[index].Indices, tgaMesh.Elements[index].MaterialIndex);
		}
		if (hasBone)
		{
			LoadSkeletonFromFBX(aPath, inOutAsset.mySkeleton);
		}
		return true;
	}
	return false;
}

bool MeshAssetHandler::LoadSkeletonFromFBX(const std::filesystem::path& aPath, SkeletonAsset& inOutAsset)
{
	TGA::FBX::Mesh mesh;
	std::wstring contentPath = L"Content\\" + aPath.wstring();
	if (TGA::FBX::Importer::LoadMeshW(contentPath, mesh))
	{
		//Translate to my own structures and assign to inOutAsset
		//Push all bones
		for (int boneIndex = 0; boneIndex < mesh.Skeleton.Bones.size(); ++boneIndex)
		{
			SkeletonAsset::Bone bone;
			bone.name = mesh.Skeleton.Bones[boneIndex].Name;
			bone.parentIndex = mesh.Skeleton.Bones[boneIndex].ParentIdx;
			CommonUtilities::TGAFBXMatrix4ToCUMatrix4(mesh.Skeleton.Bones[boneIndex].BindPoseInverse, bone.bindPoseInverse);
			//Push all children to bone
			for (int childIndex = 0; childIndex < mesh.Skeleton.Bones[boneIndex].Children.size(); ++childIndex)
			{
				bone.children.push_back(mesh.Skeleton.Bones[boneIndex].Children[childIndex]);
			}
			inOutAsset.bones.push_back(bone);
		}
		//Push all sockets to map
		for (std::pair<std::string, TGA::FBX::Skeleton::Socket> pair : mesh.Skeleton.Sockets)
		{
			Socket socket;
			socket.name = pair.second.Name;
			socket.parentBoneIndex = pair.second.ParentBoneIdx;
			CommonUtilities::TGAFBXMatrix4ToCUMatrix4(pair.second.RestTransform, socket.restTransform);
			inOutAsset.sockets.insert({ pair.first, socket });
		}
		//Push all boneName to index
		for (std::pair<std::string, size_t> pair : mesh.Skeleton.BoneNameToIndex)
		{
			inOutAsset.boneNameToIndex.insert({ pair.first, pair.second });
		}
		return true;
	}
	return false;
}

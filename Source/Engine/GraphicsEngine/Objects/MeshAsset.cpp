#include "GraphicsEngine.pch.h"


#include "GraphicsEngine.h"
#include "../../AssetManager/MeshAssethandler.h"

#include "MeshAsset.h"
#include "MaterialAsset.h"


MeshAsset::MeshAsset()
{
	myPath = L"";
	myName = L"";
}

MeshAsset::MeshAsset(const std::filesystem::path& aPath)
{
	myName = aPath.stem().wstring();
	myPath = aPath;
}



void MeshAsset::AddElement(const std::vector<Vertex>& someMdlVertices, const std::vector<UINT>& someMdlIndices, const UINT aMaterialIndex)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	if (!GraphicsEngine::GetRHI()->CreateVertexBuffer("Mesh Vertex Buffer", someMdlVertices, vertexBuffer))
	{
		return;
	}
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	if (!GraphicsEngine::GetRHI()->CreateIndexBuffer("Mesh Index buffer", someMdlIndices, indexBuffer))
	{
		return;
	}

	Element mesh = {};
	mesh.vertexBuffer = std::move(vertexBuffer);
	mesh.indexBuffer = std::move(indexBuffer);
	mesh.NumVertices = static_cast<unsigned>(someMdlVertices.size());
	mesh.NumIndices = static_cast<unsigned>(someMdlIndices.size());
	mesh.IndexOffset = 0;
	mesh.VertexOffset = 0;
	mesh.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mesh.MaterialIndex = aMaterialIndex;
	myElements.push_back(mesh);
}

bool MeshAsset::Load()
{
	if (MeshAssetHandler::Get().LoadMeshFromFBX(myPath, *this))
	{
		return true;
	}
	return false;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> MeshAsset::GetVertexBuffer(const size_t anElementIndex) const
{
	return myElements[anElementIndex].vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> MeshAsset::GetIndexBuffer(const size_t anElementIndex) const
{
	return myElements[anElementIndex].indexBuffer;
}

unsigned MeshAsset::GetPrimitiveTopology() const
{
	return myPrimitiveTopology;
}

std::wstring MeshAsset::GetName() const
{
	return myName;
}

SkeletonAsset MeshAsset::GetSkeleton() const
{

	return mySkeleton;
}


void MeshAsset::SetMaterial(const std::shared_ptr<MaterialAsset>& aMaterial, const unsigned int& anElementIndex)
{
	myElements[anElementIndex].Material = aMaterial;
}

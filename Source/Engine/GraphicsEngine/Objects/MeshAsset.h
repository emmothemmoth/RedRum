#pragma once
#include <filesystem>
#include <vector>
#include <memory>

#include "d3d11.h"
#include "d3d11_1.h"
#include <wrl/client.h>


#include "../Utilities/CommonUtilities/Matrix.hpp"

#include "Vertex.h"
#include "IAsset.h"
#include "SkeletonAsset.h"


class MaterialAsset;

class MeshAsset : public IAsset
{
public:
	struct Element
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
		unsigned VertexOffset = 0;
		unsigned IndexOffset = 0;
		unsigned NumVertices = 0;
		unsigned NumIndices = 0;
		unsigned MaterialIndex = 0;
		unsigned PrimitiveTopology = 0;
		std::weak_ptr<MaterialAsset> Material;
	};

private:
	std::vector<Element> myElements;
	std::filesystem::path myPath;
	std::wstring myName;
	unsigned myPrimitiveTopology = 0;
public:
	MeshAsset();
	MeshAsset(const std::filesystem::path& aPath);
	MeshAsset(const MeshAsset& aMesh) = default;


	void AddElement(const std::vector<Vertex>& someMdlVertices, const std::vector<UINT>& someMdlIndices, const UINT aMaterialIndex);

	bool Load() override;

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer(const size_t anElementIndex) const;
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer(const size_t anElementIndex) const;
	unsigned GetPrimitiveTopology() const;

	std::wstring GetName() const;

	SkeletonAsset GetSkeleton() const;

	void SetMaterial(const std::shared_ptr<MaterialAsset>& aMaterial, const unsigned int& anElementIndex);


public:
	SkeletonAsset mySkeleton;

	FORCEINLINE const Element& GetElement(const int& anIndex) const
	{
		return myElements[anIndex];
	}
	FORCEINLINE const std::vector<Element> GetElements() const
	{
		return myElements;
	}

};



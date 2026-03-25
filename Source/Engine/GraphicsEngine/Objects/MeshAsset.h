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
		uint8_t PartID = 0;
	};

private:
	std::vector<Element> myElements;
	std::filesystem::path myPath;
	std::wstring myName;
	unsigned myPrimitiveTopology = 0;
	CU::Vector3f myMin = { FLT_MAX, FLT_MAX, FLT_MAX };
	CU::Vector3f myMax = { -FLT_MAX,  -FLT_MAX, -FLT_MAX };
public:
	MeshAsset();
	MeshAsset(const std::filesystem::path& aPath);
	MeshAsset(const MeshAsset& aMesh) = default;


	void AddElement(const std::vector<Vertex>& someMdlVertices, const std::vector<UINT>& someMdlIndices, const UINT aMaterialIndex, uint8_t aPartID = 0);

	bool Load() override;

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer(const size_t anElementIndex) const;
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer(const size_t anElementIndex) const;
	unsigned GetPrimitiveTopology() const;

	std::wstring GetName() const;
	const std::filesystem::path& GetPath() const { return myPath; }

	SkeletonAsset GetSkeleton() const;

	void SetMaterial(const std::shared_ptr<MaterialAsset>& aMaterial, const unsigned int& anElementIndex);

	const CU::Vector3f& GetMinPoint() const { return myMin; }
	const CU::Vector3f& GetMaxPoint() const { return myMax; }
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



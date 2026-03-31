#pragma once

#pragma once

#include "wrl.h"
#include <vector>
#include <string>
#include "Vertex.h"
#include "DebugColor.h"
#include "../Buffers/VertexBuffer.h"

// Assuming this is included from your engine headers, or define it here if needed
// #include "../Buffers/VertexBuffer.h" 

struct ID3D11Buffer;

class DebugLineObject
{
public:
	DebugLineObject();
	~DebugLineObject();
	void SetIsDynamic(bool aIsDynamic) { myIsDynamic = aIsDynamic; }
	void Initialize(const std::vector<Vertex>& aVertexList, const std::vector<unsigned>& aIndexList, DebugColor aColor = DebugColor::Red, std::string_view aName = "", bool aFromMesh = false);
	void InitializeFromPoints(const std::vector<CU::Vector3f>& aPointList, std::vector<unsigned>& anIndexList, DebugColor aColor = DebugColor::Red, std::string_view aName = "");
	void Initialize();

	void AddLine(const CU::Vector3f& aStart, const CU::Vector3f& anEnd);
	void SetColor(const DebugColor aColor);

	void UpdateBuffers();
	void Reserve(unsigned aMaxVertices, unsigned aMaxIndices);

	// Updated to return your struct by const reference
	const VertexBuffer& GetVertexBuffer() const;
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() const;

	void SetTopology(const unsigned& aTopology);
	unsigned GetPrimitiveTopology() const;
	unsigned GetNumVertices() const;
	unsigned GetNumIndices()const;

	void SetName(std::string_view aDebugName);
	std::string_view GetName() const;
	void ResetLines();

private:
	void Init();
	void SetVertexColor(DebugColor aColor);

private:
	std::string myName = "";
	unsigned myNumVertices = 0;
	unsigned myVertexCapacity = 0;
	unsigned myNumIndices = 0;
	unsigned myIndexCapacity = 0;
	std::vector<Vertex> myVertices;
	std::vector<unsigned> myIndices;
	unsigned myPrimitiveTopology = 0;
	bool myIsDynamic = false;

	// Now using your struct for the vertex buffer!
	VertexBuffer myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;
};

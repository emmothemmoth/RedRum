#include "GraphicsEngine.pch.h"
#include "DebugLineObject.h"
#include "../GraphicsEngine.h"

DebugLineObject::DebugLineObject()
{
}

DebugLineObject::~DebugLineObject()
{
	myVertexBuffer.Buffer.Reset();
	myVertexBuffer.DataSize = 0;
	myVertexBuffer.NumVertices = 0;

	myIndexBuffer.Reset();
	myIndices.clear();
	myVertices.clear();
}

void DebugLineObject::Initialize(const std::vector<Vertex>& aVertexList, const std::vector<unsigned>& aIndexList, DebugColor aColor, std::string_view aName, bool aFromMesh)
{
	myName = aName;
	myVertices = aVertexList;
	myIndices = aIndexList;

	if (aFromMesh) // inefficient
	{
		std::vector<unsigned> myIndexlist;
		for (int index = 0; index < myIndices.size() - 3; index += 3)
		{
			myIndexlist.push_back(myIndices[index]);
			myIndexlist.push_back(myIndices[index + 1]);
			myIndexlist.push_back(myIndices[index + 1]);
			myIndexlist.push_back(myIndices[index + 2]);
			myIndexlist.push_back(myIndices[index + 2]);
			myIndexlist.push_back(myIndices[index]);
		}
		myIndices = myIndexlist;
	}

	myNumIndices = static_cast<unsigned>(myIndices.size());
	myNumVertices = static_cast<unsigned>(myVertices.size());

	SetVertexColor(aColor);
	Init(); // Funnel through Init to build the struct properly
}

void DebugLineObject::InitializeFromPoints(const std::vector<CU::Vector3f>& aPointList, std::vector<unsigned>& anIndexList, DebugColor aColor, std::string_view aName)
{
	myName = aName;
	myVertices.clear();
	for (auto& point : aPointList)
	{
		myVertices.emplace_back(point);
	}
	myIndices = anIndexList;
	myNumIndices = static_cast<unsigned>(myIndices.size());
	myNumVertices = static_cast<unsigned>(myVertices.size());

	SetVertexColor(aColor);
	Init(); // Funnel through Init to build the struct properly
}

void DebugLineObject::Initialize()
{
	Init();
}

void DebugLineObject::Init()
{
	if (myNumVertices == 0 || myNumIndices == 0) return;
	myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	myVertexCapacity = myNumVertices;
	myIndexCapacity = myNumIndices;

	myVertexBuffer.Buffer.Reset();
	myIndexBuffer.Reset();

	// Pass myVertexBuffer.Buffer (the ComPtr) to your creation function
	GraphicsEngine::Get().CreateVertexBuffer(myName + " vertex buffer", myVertices, myVertexBuffer.Buffer, myIsDynamic);

	// Populate the rest of your custom struct!
	myVertexBuffer.Name = myName + " vertex buffer";
	myVertexBuffer.NumVertices = myVertexCapacity;
	myVertexBuffer.VertexStride = sizeof(Vertex);
	myVertexBuffer.DataSize = myVertexCapacity * sizeof(Vertex);

	GraphicsEngine::Get().CreateIndexBuffer(myName + " index buffer", myIndices, myIndexBuffer, myIsDynamic);
}

void DebugLineObject::Reserve(unsigned aMaxVertices, unsigned aMaxIndices)
{
	if (aMaxVertices == 0 || aMaxIndices == 0) return;
	if (aMaxVertices <= myVertexCapacity && aMaxIndices <= myIndexCapacity)
		return;

	// Replaced std::max with simple ternary operators
	myVertexCapacity = aMaxVertices > myVertexCapacity ? aMaxVertices : myVertexCapacity;
	myIndexCapacity = aMaxIndices > myIndexCapacity ? aMaxIndices : myIndexCapacity;

	std::vector<Vertex> dummyVertices(myVertexCapacity);
	std::vector<unsigned> dummyIndices(myIndexCapacity);

	myVertexBuffer.Buffer.Reset();
	myIndexBuffer.Reset();

	myIsDynamic = true;

	// Pass the internal ComPtr
	GraphicsEngine::Get().CreateVertexBuffer(myName + " vertex buffer", dummyVertices, myVertexBuffer.Buffer, myIsDynamic);

	// Keep the struct metadata synced
	myVertexBuffer.Name = myName + " vertex buffer";
	myVertexBuffer.NumVertices = myVertexCapacity;
	myVertexBuffer.VertexStride = sizeof(Vertex);
	myVertexBuffer.DataSize = myVertexCapacity * sizeof(Vertex);

	GraphicsEngine::Get().CreateIndexBuffer(myName + " index buffer", dummyIndices, myIndexBuffer, myIsDynamic);
}

void DebugLineObject::UpdateBuffers()
{
	if (myVertices.empty() || myIndices.empty())
		return;

	// Check if we need to resize
	if (myNumVertices > myVertexCapacity || myNumIndices > myIndexCapacity)
	{
		// Replaced std::max with simple ternary operators
		unsigned nextVCap = myNumVertices > (myVertexCapacity * 2) ? myNumVertices : (myVertexCapacity * 2);
		unsigned nextICap = myNumIndices > (myIndexCapacity * 2) ? myNumIndices : (myIndexCapacity * 2);
		Reserve(nextVCap, nextICap);
	}

	// Because myVertexBuffer is now the struct your RHI expects, 
	// you can just pass the whole struct directly into your engine!
	GraphicsEngine::Get().UpdateVertexBuffer(myVertexBuffer, myVertices);
	GraphicsEngine::Get().UpdateIndexBuffer(myIndexBuffer, myIndices);
}

void DebugLineObject::AddLine(const CU::Vector3f& aStart, const CU::Vector3f& anEnd)
{
	myIndices.emplace_back(static_cast<unsigned>(myVertices.size()));
	myVertices.emplace_back(aStart);
	myIndices.emplace_back(static_cast<unsigned>(myVertices.size()));
	myVertices.emplace_back(anEnd);

	myNumIndices = static_cast<unsigned>(myIndices.size());
	myNumVertices = static_cast<unsigned>(myVertices.size());
}

void DebugLineObject::SetColor(const DebugColor aColor)
{
	SetVertexColor(aColor);
}

const VertexBuffer& DebugLineObject::GetVertexBuffer() const
{
	return myVertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> DebugLineObject::GetIndexBuffer() const
{
	return myIndexBuffer;
}

void DebugLineObject::SetTopology(const unsigned& aTopology)
{
	myPrimitiveTopology = aTopology;
}

unsigned DebugLineObject::GetPrimitiveTopology() const
{
	return myPrimitiveTopology;
}

unsigned DebugLineObject::GetNumVertices() const
{
	return myNumVertices;
}

unsigned DebugLineObject::GetNumIndices() const
{
	return myNumIndices;
}

void DebugLineObject::SetName(std::string_view aDebugName)
{
	myName = aDebugName;
}

std::string_view DebugLineObject::GetName() const
{
	return myName;
}

void DebugLineObject::ResetLines()
{
	myVertices.clear();
	myIndices.clear();
	myNumIndices = 0;
	myNumVertices = 0;
	if (myVertexBuffer.Buffer == nullptr || myIndexBuffer == nullptr)
	{
		Init();
	}
}

void DebugLineObject::SetVertexColor(DebugColor aColor)
{
	for (auto& vertex : myVertices)
	{
		switch (aColor)
		{
		case DebugColor::White: vertex.Color = { 1.0f, 1.0f, 1.0f, 1.0f }; break;
		case DebugColor::Red: vertex.Color = { 1.0f, 0.0f, 0.0f, 1.0f }; break;
		case DebugColor::Green: vertex.Color = { 0.0f, 1.0f, 0.0f, 1.0f }; break;
		case DebugColor::Blue: vertex.Color = { 0.0f, 0.0f, 1.0f, 1.0f }; break;
		case DebugColor::Yellow: vertex.Color = { 1.0f, 1.0f, 0.0f, 1.0f }; break;
		case DebugColor::Orange: vertex.Color = { 1.0f, 0.3f, 0.0f, 1.0f }; break;
		case DebugColor::Purple: vertex.Color = { 0.3f, 0.0f, 0.8f, 1.0f }; break;
		case DebugColor::Pink: vertex.Color = { 1.0f, 0.0f, 1.0f, 1.0f }; break;
		default: break;
		}
	}
}
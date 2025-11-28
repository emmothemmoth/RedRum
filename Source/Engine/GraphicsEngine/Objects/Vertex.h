#pragma once


#include "../InterOp/RHIStructs.h"
#include "../../Utilities/CommonUtilities/Vector.hpp"

#include <wrl.h>
#include <vector>
#include <d3d11.h>

using namespace Microsoft::WRL;

// We'll be writing this a lot, so easier
// to just typedef it here.
typedef std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementList;
typedef CommonUtilities::Vector4<float> RGBf;
typedef CommonUtilities::Vector3<float> Vector3f;
typedef CommonUtilities::Vector2<float> Vector2f;
typedef CommonUtilities::Vector4<float> Vector4f;
typedef CommonUtilities::Vector4<unsigned int> Vector4U;

struct Vertex
{
	Vector4f Position;
	RGBf Color;
	Vector3f Normal = { 0.0f, 0.0f, 0.0f };
	Vector3f Tangent = { 0.0f, 0.0f, 0.0f };
	Vector4U BoneIDs = { 0,0,0,0 };
	Vector4f BoneWeights = { 0.0f, 0.0f, 0.0f, 0.0f };
	Vector2f myUV = { 0.0f, 0.0f };
	Vector2f myUV1 = { 0.0f, 0.0f };
	Vertex(float anX, float aY, float aZ, float anR, float aG, float aB, float anAlpha = 1)
	{
		Position.x = anX;
		Position.y = aY;
		Position.z = aZ;
		Position.w = 1.0f;

		Color.x = anR;
		Color.y = aG;
		Color.z = aB;
		Color.w = anAlpha;
	}
	Vertex(float anX, float aY, float aZ, float aNormalX, float aNormalY, float aNormalZ, float aTangentX, float aTangentY, float aTangentZ)
	{
		Position.x = anX;
		Position.y = aY;
		Position.z = aZ;
		Position.w = 1.0f;

		Normal.x = aNormalX;
		Normal.y = aNormalY;
		Normal.z = aNormalZ;

		Tangent.x = aTangentX;
		Tangent.y = aTangentY;
		Tangent.z = aTangentZ;
	}

	Vertex(Vector3f aPosition, Vector3f aNormal, Vector3f aTangent)
	{
		Position.x = aPosition.x;
		Position.y = aPosition.y;
		Position.z = aPosition.z;
		Position.w = 1.0f;

		Normal = aNormal;

		Tangent = aTangent;
	}
	Vertex(Vector3f aPosition)
	{
		Position.x = aPosition.x;
		Position.y = aPosition.y;
		Position.z = aPosition.z;
		Position.w = 1.0f;
	}
	Vertex(Vector3f aPosition,Vector2f aUV, Vector3f aNormal, Vector3f aTangent)
	{
		Position.x = aPosition.x;
		Position.y = aPosition.y;
		Position.z = aPosition.z;
		Position.w = 1.0f;

		myUV = aUV;

		Normal = aNormal;

		Tangent = aTangent;
	}

	static const std::vector<VertexElementDesc> InputLayoutDefinition;
};


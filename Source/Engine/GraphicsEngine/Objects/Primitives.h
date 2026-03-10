#pragma once
#include "MeshAsset.h"
#include "CommonUtilities\Plane.hpp"
#include "CommonUtilities\Vector3.hpp"
#define PI 3.14159265358979323846f
typedef CommonUtilities::Vector3<float> Vector3f;

struct PrimitiveElement
{
	std::vector<Vertex> Vertices;
	std::vector<unsigned> Indices;
	unsigned MaterialIndex = 0;
	uint8_t PartID = 0;
};

struct PrimitiveMesh
{
	std::vector<PrimitiveElement> Elements;
};

struct CubeData : PrimitiveMesh
{
	CubeData()
	{
		Elements.resize(1);
		auto& e = Elements[0];

		e.Vertices =
		{
			{ Vector3f(50,50,50), {1,0}, {1,0,0}, {0,0,1} },
			{ Vector3f(50,50,-50), {0,0}, {1,0,0}, {0,0,1} },
			{ Vector3f(50,-50,50), {1,1}, {1,0,0}, {0,0,1} },
			{ Vector3f(50,-50,-50), {0,1}, {1,0,0}, {0,0,1} },

			{ Vector3f(-50,50,50), {0,0}, {0,1,0}, {1,0,0} },
			{ Vector3f(-50,50,-50), {0,1}, {0,1,0}, {1,0,0} },
			{ Vector3f(50,50,50), {1,0}, {0,1,0}, {1,0,0} },
			{ Vector3f(50,50,-50), {1,1}, {0,1,0}, {1,0,0} },

			{ Vector3f(-50,-50,50), {0,1}, {0,-1,0}, {1,0,0} },
			{ Vector3f(-50,-50,-50), {0,0}, {0,-1,0}, {1,0,0} },
			{ Vector3f(50,-50,50), {1,1}, {0,-1,0}, {1,0,0} },
			{ Vector3f(50,-50,-50), {1,0}, {0,-1,0}, {1,0,0} },

			{ Vector3f(-50,50,-50), {0,0}, {0,0,-1}, {1,0,0} },
			{ Vector3f(-50,-50,-50), {0,1}, {0,0,-1}, {1,0,0} },
			{ Vector3f(50,50,-50), {1,0}, {0,0,-1}, {1,0,0} },
			{ Vector3f(50,-50,-50), {1,1}, {0,0,-1}, {1,0,0} },

			{ Vector3f(-50,50,50), {1,0}, {0,0,1}, {-1,0,0} },
			{ Vector3f(-50,-50,50), {1,1}, {0,0,1}, {-1,0,0} },
			{ Vector3f(50,50,50), {0,0}, {0,0,1}, {-1,0,0} },
			{ Vector3f(50,-50,50), {0,1}, {0,0,1}, {-1,0,0} },

			{ Vector3f(-50,50,50), {0,0}, {-1,0,0}, {0,0,-1} },
			{ Vector3f(-50,-50,50), {0,1}, {-1,0,0}, {0,0,-1} },
			{ Vector3f(-50,50,-50), {1,0}, {-1,0,0}, {0,0,-1} },
			{ Vector3f(-50,-50,-50), {1,1}, {-1,0,0}, {0,0,-1} }
		};

		e.Indices =
		{
			0,2,1,1,2,3,
			16,17,19,19,18,16,
			22,23,20,20,23,21,
			14,15,12,12,15,13,
			6,7,4,4,7,5,
			11,10,9,9,10,8
		};
	}
};

struct GroundData : PrimitiveMesh
{
	GroundData()
	{
		Elements.resize(1);
		auto& e = Elements[0];

		e.Vertices =
		{
			{ {-10000,0,-10000},{0,1},{0,1,0},{1,0,0} },
			{ {-10000,0,10000},{0,0},{0,1,0},{1,0,0} },
			{ {10000,0,10000},{1,0},{0,1,0},{1,0,0} },

			{ {-10000,0,-10000},{0,1},{0,1,0},{1,0,0} },
			{ {10000,0,-10000},{1,1},{0,1,0},{1,0,0} },
			{ {10000,0,10000},{1,0},{0,1,0},{1,0,0} }
		};

		e.Indices =
		{
			2,0,1,
			5,4,3
		};
	}
};

struct SphereData : public PrimitiveMesh
{
	SphereData(float radius = 50.f, int slices = 24, int stacks = 16)
	{
		Elements.resize(1);
		auto& e = Elements[0];

		for (int stack = 0; stack <= stacks; ++stack)
		{
			float v = (float)stack / stacks;
			float phi = v * 3.14159265359f;

			for (int slice = 0; slice <= slices; ++slice)
			{
				float u = (float)slice / slices;
				float theta = u * 2.0f * 3.14159265359f;

				float x = std::sin(phi) * std::cos(theta);
				float y = std::cos(phi);
				float z = std::sin(phi) * std::sin(theta);

				Vector3f normal = { x, y, z };
				Vector3f pos = normal * radius;

				e.Vertices.push_back({
					pos,
					{ u, v },
					normal,
					{1,0,0}
					});
			}
		}

		for (int stack = 0; stack < stacks; ++stack)
		{
			for (int slice = 0; slice < slices; ++slice)
			{
				int first = stack * (slices + 1) + slice;
				int second = first + slices + 1;

				e.Indices.push_back(first);
				e.Indices.push_back(second);
				e.Indices.push_back(first + 1);

				e.Indices.push_back(second);
				e.Indices.push_back(second + 1);
				e.Indices.push_back(first + 1);
			}
		}

		e.MaterialIndex = 0;
	}
};

struct HorizontalPlaneData : PrimitiveMesh
{
	HorizontalPlaneData(float size = 200.f)
	{
		Elements.resize(1);
		auto& e = Elements[0];

		float h = size * 0.5f;

		e.Vertices =
		{
			{ {h,0,h},{0,1},{0,1,0},{-1,0,0} },
			{ {h,0,-h},{0,0},{0,1,0},{-1,0,0} },
			{ {-h,0,h},{1,1},{0,1,0},{-1,0,0} },
			{ {-h,0,-h},{1,0},{0,1,0},{-1,0,0} }
		};

		e.Indices =
		{
			0,1,2,
			3,2,1
		};
	}
};

struct VerticalPlaneData : PrimitiveMesh
{
	VerticalPlaneData(float size = 200.f)
	{
		Elements.resize(1);
		auto& e = Elements[0];

		float h = size * 0.5f;

		e.Vertices =
		{
			{ {h,h,0},{0,1},{0,0,1},{-1,0,0} },
			{ {h,-h,0},{0,0},{0,0,1},{-1,0,0} },
			{ {-h,h,0},{1,1},{0,0,1},{-1,0,0} },
			{ {-h,-h,0},{1,0},{0,0,1},{-1,0,0} }
		};

		e.Indices =
		{
			1,2,3,
			2,1,0
		};
	}
};

struct TransformGizmoData : PrimitiveMesh
{
	TransformGizmoData()
	{
		Elements.resize(3);

		BuildAxis(Elements[0], Axis::X);
		BuildAxis(Elements[1], Axis::Y);
		BuildAxis(Elements[2], Axis::Z);

		Elements.at(0).PartID = 1;
		Elements.at(1).PartID = 2;
		Elements.at(2).PartID = 3;
	}

private:

	enum class Axis
	{
		X,
		Y,
		Z
	};

	// ------------------------------------------------------------

	float GetAxisU(Axis axis)
	{
		switch (axis)
		{
		case Axis::X: return 0.16f;
		case Axis::Y: return 0.50f;
		case Axis::Z: return 0.83f;
		}
		return 0.5f;
	}

	// ------------------------------------------------------------

	CommonUtilities::Vector4f MakePosition(Axis axis, float a, float b, float c)
	{
		switch (axis)
		{
		case Axis::X: return { a, b, c, 1.0f };
		case Axis::Y: return { b, a, c, 1.0f };
		case Axis::Z: return { b, c, a, 1.0f };
		}

		return { 0,0,0,1 };
	}

	// ------------------------------------------------------------

	void BuildAxis(PrimitiveElement& e, Axis axis)
	{
		const float shaftLength = 100.0f;
		const float coneLength = 50.0f;

		const float shaftRadius = 4.0f;
		const float coneRadius = 13.0f;

		const int segments = 16;

		BuildCylinder(e, axis, shaftLength, shaftRadius, segments);
		BuildCone(e, axis, shaftLength, coneLength, coneRadius, segments);
	}

	// ------------------------------------------------------------

	void BuildCylinder(
		PrimitiveElement& e,
		Axis axis,
		float length,
		float radius,
		int segments)
	{
		float u = GetAxisU(axis);

		unsigned baseIndex = (unsigned)e.Vertices.size();

		for (int i = 0; i < segments; i++)
		{
			float a0 = (float)i / segments * 2.0f * PI;
			float a1 = (float)(i + 1) / segments * 2.0f *PI;

			float c0 = cosf(a0) * radius;
			float s0 = sinf(a0) * radius;

			float c1 = cosf(a1) * radius;
			float s1 = sinf(a1) * radius;

			auto p0 = MakePosition(axis, 0, c0, s0);
			auto p1 = MakePosition(axis, length, c0, s0);

			auto p2 = MakePosition(axis, 0, c1, s1);
			auto p3 = MakePosition(axis, length, c1, s1);

			Vertex v0; v0.Position = p0; v0.myUV = { u,0.5f };
			Vertex v1; v1.Position = p1; v1.myUV = { u,0.5f };
			Vertex v2; v2.Position = p2; v2.myUV = { u,0.5f };
			Vertex v3; v3.Position = p3; v3.myUV = { u,0.5f };

			e.Vertices.push_back(v0);
			e.Vertices.push_back(v1);
			e.Vertices.push_back(v2);
			e.Vertices.push_back(v3);

			unsigned i0 = baseIndex + i * 4 + 0;
			unsigned i1 = baseIndex + i * 4 + 1;
			unsigned i2 = baseIndex + i * 4 + 2;
			unsigned i3 = baseIndex + i * 4 + 3;

			e.Indices.push_back(i0);
			e.Indices.push_back(i1);
			e.Indices.push_back(i2);

			e.Indices.push_back(i2);
			e.Indices.push_back(i1);
			e.Indices.push_back(i3);
		}
	}

	// ------------------------------------------------------------

	void BuildCone(
		PrimitiveElement& e,
		Axis axis,
		float basePos,
		float length,
		float radius,
		int segments)
	{
		float u = GetAxisU(axis);

		unsigned baseIndex = (unsigned)e.Vertices.size();

		CommonUtilities::Vector4f tip;

		switch (axis)
		{
		case Axis::X: tip = { basePos + length,0,0,1 }; break;
		case Axis::Y: tip = { 0,basePos + length,0,1 }; break;
		case Axis::Z: tip = { 0,0,basePos + length,1 }; break;
		}

		// Tip
		Vertex tipV;
		tipV.Position = tip;
		tipV.myUV = { u,0.5f };

		e.Vertices.push_back(tipV);

		// Ring vertices
		for (int i = 0; i < segments; i++)
		{
			float a = (float)i / segments * 2.0f * PI;

			float c = cosf(a) * radius;
			float s = sinf(a) * radius;

			auto p = MakePosition(axis, basePos, c, s);

			Vertex v;
			v.Position = p;
			v.myUV = { u,0.5f };

			e.Vertices.push_back(v);
		}

		// Cone sides
		for (int i = 0; i < segments; i++)
		{
			unsigned tipIndex = baseIndex;
			unsigned v0 = baseIndex + 1 + i;
			unsigned v1 = baseIndex + 1 + ((i + 1) % segments);

			if (axis == Axis::Y)
			{
				e.Indices.push_back(tipIndex);
				e.Indices.push_back(v1);
				e.Indices.push_back(v0);
			}
			else
			{
				e.Indices.push_back(tipIndex);
				e.Indices.push_back(v0);
				e.Indices.push_back(v1);
			}
		}

		// -------- NEW: Base cap --------

		unsigned centerIndex = (unsigned)e.Vertices.size();

		auto centerPos = MakePosition(axis, basePos, 0, 0);

		Vertex center;
		center.Position = centerPos;
		center.myUV = { u,0.5f };

		e.Vertices.push_back(center);

		for (int i = 0; i < segments; i++)
		{
			unsigned v0 = baseIndex + 1 + i;
			unsigned v1 = baseIndex + 1 + ((i + 1) % segments);

			if (axis == Axis::Y)
			{
				e.Indices.push_back(centerIndex);
				e.Indices.push_back(v0);
				e.Indices.push_back(v1);
			}
			else
			{
				e.Indices.push_back(centerIndex);
				e.Indices.push_back(v1);
				e.Indices.push_back(v0);
			}
		}
	}
};
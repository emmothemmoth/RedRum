#pragma once
#include "MeshAsset.h"
#include "CommonUtilities\Plane.hpp"
#include "CommonUtilities\Vector3.hpp"

typedef CommonUtilities::Vector3<float> Vector3f;

struct PrimitiveElement
{
	std::vector<Vertex> Vertices;
	std::vector<unsigned> Indices;
	unsigned MaterialIndex = 0;
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

		BuildX();
		BuildY();
		BuildZ();
	}

private:

	void BuildX()
	{
		auto& e = Elements[0];

		e.Vertices =
		{
			{ {0,0,0},{0.16f,0.5f},{1,0,0},{0,1,0} },
			{ {100,0,0},{0.16f,0.5f},{1,0,0},{0,1,0} },
			{ {80,4,4},{0.16f,0.5f},{1,0,0},{0,1,0} },
			{ {80,-4,4},{0.16f,0.5f},{1,0,0},{0,1,0} },
			{ {80,4,-4},{0.16f,0.5f},{1,0,0},{0,1,0} },
			{ {80,-4,-4},{0.16f,0.5f},{1,0,0},{0,1,0} }
		};

		e.Indices =
		{
			// Shaft (Connecting origin to base)
			0,3,2,  0,4,3,  0,5,4,  0,2,5,

			// Pyramid Head (Connecting base to the Tip at index 1)
			1,2,3,  1,3,4,  1,4,5,  1,5,2,

			// Base Cap (Optional, closes the bottom of the arrow head)
			2,4,3,  2,5,4
		};
	}

	void BuildY()
	{
		auto& e = Elements[1];

		e.Vertices =
		{
			{ {0,0,0},{0.5f,0.5f},{0,1,0},{1,0,0} },
			{ {0,100,0},{0.5f,0.5f},{0,1,0},{1,0,0} },
			{ {4,80,4},{0.5f,0.5f},{0,1,0},{1,0,0} },
			{ {-4,80,4},{0.5f,0.5f},{0,1,0},{1,0,0} },
			{ {4,80,-4},{0.5f,0.5f},{0,1,0},{1,0,0} },
			{ {-4,80,-4},{0.5f,0.5f},{0,1,0},{1,0,0} }
		};

		e.Indices =
		{
			// Shaft
			0,2,3,  0,3,4,  0,4,5,  0,5,2,
			// Head (Tip is index 1)
			1,3,2,  1,4,3,  1,5,4,  1,2,5,
			// Base Cap
			2,5,4,  2,4,3
		};
	}

	void BuildZ()
	{
		auto& e = Elements[2];

		e.Vertices =
		{
			{ {0,0,0},{0.83f,0.5f},{0,0,1},{1,0,0} },
			{ {0,0,100},{0.83f,0.5f},{0,0,1},{1,0,0} },
			{ {4,4,80},{0.83f,0.5f},{0,0,1},{1,0,0} },
			{ {-4,4,80},{0.83f,0.5f},{0,0,1},{1,0,0} },
			{ {4,-4,80},{0.83f,0.5f},{0,0,1},{1,0,0} },
			{ {-4,-4,80},{0.83f,0.5f},{0,0,1},{1,0,0} }
		};

		e.Indices =
		{
			// Shaft
			0,3,2,  0,4,3,  0,5,4,  0,2,5,
			// Head (Tip is index 1)
			1,2,3,  1,3,4,  1,4,5,  1,5,2,
			// Base Cap
			2,4,3,  2,5,4
		};
	}
};
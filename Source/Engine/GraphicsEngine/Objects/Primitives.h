#pragma once
#include "MeshAsset.h"
#include "CommonUtilities\Plane.hpp"
#include "CommonUtilities\Vector3.hpp"

typedef CommonUtilities::Vector3<float> Vector3f;

struct CubeData
{
	CommonUtilities::Matrix4x4<float> transform;

	//With normals
	std::vector<Vertex> mdlVertices
	{
		{
			//RIGHT SIDE
				Vector3f(50.0f, 50.0f, 50.0f), //topRightBack
				Vector2f(1.0f, 0.0f),
				Vector3f(1.0f, 0.0f, 0.0f),
				Vector3f(0.0f, 0.0f, 1.0f)
			},
			{
				Vector3f(50.0f, 50.0f, -50.0f), //toprightfront
				Vector2f(0.0f, 0.0f),
				Vector3f(1.0f, 0.0f, 0.0f),
				Vector3f(0.0f, 0.0f, 1.0f)
			},
			{
				Vector3f(50.0f, -50.0f, 50.0f), //bottomrightback
				Vector2f(1.0f, 1.0f),
				Vector3f(1.0f, 0.0f, 0.0f),
				Vector3f(0.0f, 0.0f, 1.0f)
			},
			{
				Vector3f(50.0f, -50.0f, -50.0f), //bottomrightfront
				Vector2f(0.0f, 1.0f),
				Vector3f(1.0f, 0.0f, 0.0f),
				Vector3f(0.0f, 0.0f, 1.0f)
			},
		//TOP
		{
			Vector3f(-50.0f, 50.0f, 50.0f), //topleftBack
			Vector2f(0.0f, 0.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(-50.0f, 50.0f, -50.0f), //topleftfront
			Vector2f(0.0f, 1.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, 50.0f, 50.0f), //toprightback
			Vector2f(1.0f, 0.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, 50.0f, -50.0f), //toprightfront
			Vector2f(1.0f, 1.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		//Bottom
		{
			Vector3f(-50.0f, -50.0f, 50.0f), //bottomleftback
			Vector2f(0.0f, 1.0f),
			Vector3f(0.0f, -1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(-50.0f, -50.0f, -50.0f), //bottomleftfront
			Vector2f(0.0f, 0.0f),
			Vector3f(0.0f, -1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, -50.0f, 50.0f), //bottomrightback
			Vector2f(1.0f, 1.0f),
			Vector3f(0.0f, -1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, -50.0f, -50.0f), //bottomrightfront
			Vector2f(1.0f, 0.0f),
			Vector3f(0.0f, -1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		//FRONT
		{
			Vector3f(-50.0f, 50.0f, -50.0f), //topleftfront
			Vector2f(0.0f, 0.0f),
			Vector3f(0.0f, 0.0f , -1.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(-50.0f, -50.0f, -50.0f), //bottomleftfront
			Vector2f(0.0f, 1.0f),
			Vector3f(0.0f, 0.0f , -1.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, 50.0f, -50.0f), //toprightfront
			Vector2f(1.0f, 0.0f),
			Vector3f(0.0f, 0.0f , -1.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, -50.0f, -50.0f), //bottomrightfront
			Vector2f(1.0f, 1.0f),
			Vector3f(0.0f, 0.0f , -1.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		//BACK
		{
			Vector3f(-50.0f, 50.0f, 50.0f), //topleftback
			Vector2f(1.0f, 0.0f),
			Vector3f(0.0f, 0.0f , 1.0f),
			Vector3f(-1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(-50.0f, -50.0f, 50.0f), //bottomleftback
			Vector2f(1.0f, 1.0f),
			Vector3f(0.0f, 0.0f , 1.0f),
			Vector3f(-1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, 50.0f, 50.0f), //toprightback
			Vector2f(0.0f, 0.0f),
			Vector3f(0.0f, 0.0f , 1.0f),
			Vector3f(-1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, -50.0f, 50.0f), //bottomrightback
			Vector2f(0.0f, 1.0f),
			Vector3f(0.0f, 0.0f , 1.0f),
			Vector3f(-1.0f, 0.0f, 0.0f)
		},
		//LEFT SIDE
		{
			Vector3f(-50.0f, 50.0f, 50.0f), //topleftback
			Vector2f(0.0f, 0.0f),
			Vector3f(-1.0f, 0.0f, 0.0f),
			Vector3f(0.0f, 0.0f, -1.0f)
		},
		{
			Vector3f(-50.0f, -50.0f, 50.0f), //bottomleftback
			Vector2f(0.0f, 1.0f),
			Vector3f(-1.0f, 0.0f, 0.0f),
			Vector3f(0.0f, 0.0f, -1.0f)
		},
		{
			Vector3f(-50.0f, 50.0f, -50.0f), //topleftfront
			Vector2f(1.0f, 0.0f),
			Vector3f(-1.0f, 0.0f, 0.0f),
			Vector3f(0.0f, 0.0f, -1.0f)
		},
		{
			Vector3f(-50.0f, -50.0f, -50.0f), //bottomleftfront
			Vector2f(1.0f, 1.0f),
			Vector3f(-1.0f, 0.0f, 0.0f),
			Vector3f(0.0f, 0.0f, -1.0f)
		},

	};

	std::vector<UINT> mdlIndices =
	{
		0,2,1, //right side
		1,2,3,
		16,17,19,//Back side
		19,18,16,
		22,23,20, //Left side
		20,23,21,
		14,15,12, //Front side
		12,15,13,
		6,7,4, //Top
		4,7,5,
		11,10,9, //Bottom
		9,10,8

	};
};

struct GroundData
{
	std::vector<Vertex> mdlVertices
	{
		//First
		{
			Vector3f(-10000.0f, 0.0f, -10000.0f),
			Vector2f(0.0f, 1.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(-10000.0f, 0.0f, 10000.0f),
			Vector2f(0.0f, 0.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(10000.0f, 0.0f, 10000.0f),
			Vector2f(1.0f, 0.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		//Second
		{
			Vector3f(-10000.0f, 0.0f, -10000.0f),
			Vector2f(0.0f, 1.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(10000.0f, 0.0f, -10000.0f),
			Vector2f(1.0f, 1.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(10000.0f, 0.0f, 10000.0f),
			Vector2f(1.0f, 0.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		}
	};

	std::vector<UINT> mdlIndices =
	{
		2, 0, 1,
		5, 4, 3
	};
};

struct SphereData
{
	std::vector<Vertex> mdlVertices
	{
		{
			//RIGHT SIDE
				Vector3f(50.0f, 50.0f, 50.0f), //topRightBack
				Vector2f(1.0f, 0.0f),
				Vector3f(1.0f, 0.0f, 0.0f),
				Vector3f(0.0f, 0.0f, 1.0f)
			},
			{
				Vector3f(50.0f, 50.0f, -50.0f), //toprightfront
				Vector2f(0.0f, 0.0f),
				Vector3f(1.0f, 0.0f, 0.0f),
				Vector3f(0.0f, 0.0f, 1.0f)
			},
			{
				Vector3f(50.0f, -50.0f, 50.0f), //bottomrightback
				Vector2f(1.0f, 1.0f),
				Vector3f(1.0f, 0.0f, 0.0f),
				Vector3f(0.0f, 0.0f, 1.0f)
			},
			{
				Vector3f(50.0f, -50.0f, -50.0f), //bottomrightfront
				Vector2f(0.0f, 1.0f),
				Vector3f(1.0f, 0.0f, 0.0f),
				Vector3f(0.0f, 0.0f, 1.0f)
			},
		//TOP
		{
			Vector3f(-50.0f, 50.0f, 50.0f), //topleftBack
			Vector2f(0.0f, 0.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(-50.0f, 50.0f, -50.0f), //topleftfront
			Vector2f(0.0f, 1.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, 50.0f, 50.0f), //toprightback
			Vector2f(1.0f, 0.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, 50.0f, -50.0f), //toprightfront
			Vector2f(1.0f, 1.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		//Bottom
		{
			Vector3f(-50.0f, -50.0f, 50.0f), //bottomleftback
			Vector2f(0.0f, 1.0f),
			Vector3f(0.0f, -1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(-50.0f, -50.0f, -50.0f), //bottomleftfront
			Vector2f(0.0f, 0.0f),
			Vector3f(0.0f, -1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, -50.0f, 50.0f), //bottomrightback
			Vector2f(1.0f, 1.0f),
			Vector3f(0.0f, -1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, -50.0f, -50.0f), //bottomrightfront
			Vector2f(1.0f, 0.0f),
			Vector3f(0.0f, -1.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		//FRONT
		{
			Vector3f(-50.0f, 50.0f, -50.0f), //topleftfront
			Vector2f(0.0f, 0.0f),
			Vector3f(0.0f, 0.0f , -1.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(-50.0f, -50.0f, -50.0f), //bottomleftfront
			Vector2f(0.0f, 1.0f),
			Vector3f(0.0f, 0.0f , -1.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, 50.0f, -50.0f), //toprightfront
			Vector2f(1.0f, 0.0f),
			Vector3f(0.0f, 0.0f , -1.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, -50.0f, -50.0f), //bottomrightfront
			Vector2f(1.0f, 1.0f),
			Vector3f(0.0f, 0.0f , -1.0f),
			Vector3f(1.0f, 0.0f, 0.0f)
		},
		//BACK
		{
			Vector3f(-50.0f, 50.0f, 50.0f), //topleftback
			Vector2f(1.0f, 0.0f),
			Vector3f(0.0f, 0.0f , 1.0f),
			Vector3f(-1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(-50.0f, -50.0f, 50.0f), //bottomleftback
			Vector2f(1.0f, 1.0f),
			Vector3f(0.0f, 0.0f , 1.0f),
			Vector3f(-1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, 50.0f, 50.0f), //toprightback
			Vector2f(0.0f, 0.0f),
			Vector3f(0.0f, 0.0f , 1.0f),
			Vector3f(-1.0f, 0.0f, 0.0f)
		},
		{
			Vector3f(50.0f, -50.0f, 50.0f), //bottomrightback
			Vector2f(0.0f, 1.0f),
			Vector3f(0.0f, 0.0f , 1.0f),
			Vector3f(-1.0f, 0.0f, 0.0f)
		},
		//LEFT SIDE
		{
			Vector3f(-50.0f, 50.0f, 50.0f), //topleftback
			Vector2f(0.0f, 0.0f),
			Vector3f(-1.0f, 0.0f, 0.0f),
			Vector3f(0.0f, 0.0f, -1.0f)
		},
		{
			Vector3f(-50.0f, -50.0f, 50.0f), //bottomleftback
			Vector2f(0.0f, 1.0f),
			Vector3f(-1.0f, 0.0f, 0.0f),
			Vector3f(0.0f, 0.0f, -1.0f)
		},
		{
			Vector3f(-50.0f, 50.0f, -50.0f), //topleftfront
			Vector2f(1.0f, 0.0f),
			Vector3f(-1.0f, 0.0f, 0.0f),
			Vector3f(0.0f, 0.0f, -1.0f)
		},
		{
			Vector3f(-50.0f, -50.0f, -50.0f), //bottomleftfront
			Vector2f(1.0f, 1.0f),
			Vector3f(-1.0f, 0.0f, 0.0f),
			Vector3f(0.0f, 0.0f, -1.0f)
		},

	};

	std::vector<UINT> mdlIndices =
	{
		0,2,1, //right side
		1,2,3,
		16,17,19,//Back side
		19,18,16,
		22,23,20, //Left side
		20,23,21,
		14,15,12, //Front side
		12,15,13,
		6,7,4, //Top
		4,7,5,
		11,10,9, //Bottom
		9,10,8

	};
};

struct HorizontalPlaneData
{
	float aSize = 200.0f;
	float halfSize = aSize / 2.f;
	std::vector<Vertex> mdlVertices = {
		{
			Vector3f(halfSize, 0.f, halfSize),
			Vector2f(0.f, 1.f),
			Vector3f(0.f, 1.f, 0.f),
			Vector3f(-1.f, 0.f, 0.f)
		},
		{
			Vector3f(halfSize, 0.f, -halfSize),
			Vector2f(0.f, 0.f),
			Vector3f(0.f, 1.f, 0.f),
			Vector3f(-1.f, 0.f, 0.f)
		},
		{
			Vector3f(-halfSize, 0.f, halfSize),
			Vector2f(1.f, 1.f),
			Vector3f(0.f, 1.f, 0.f),
			Vector3f(-1.f, 0.f, 0.f)
		},
		{
			Vector3f(-halfSize, 0.f, -halfSize),
			Vector2f(1.f, 0.f),
			Vector3f(0.f, 1.f, 0.f),
			Vector3f(-1.f, 0.f, 0.f)
		},
	};

	std::vector<unsigned> mdlIndices = {
		0, 1, 2,
		3, 2, 1
	};
};

struct VerticalPlaneData
{
	float aSize = 200.0f;
	float halfSize = aSize / 2.f;
	std::vector<Vertex> mdlVertices = {
		{
			Vector3f(halfSize, halfSize, 0.f),
			Vector2f(0.f, 1.f),
			Vector3f(0.f, 0.f, 1.f),
			Vector3f(-1.f, 0.f, 0.f)
		},
		{
			Vector3f(halfSize, -halfSize, 0.f),
			Vector2f(0.f, 0.f),
			Vector3f(0.f, 0.f, 1.f),
			Vector3f(-1.f, 0.f, 0.f)
		},
		{
			Vector3f(-halfSize, halfSize, 0.f),
			Vector2f(1.f, 1.f),
			Vector3f(0.f, 0.f, 1.f),
			Vector3f(-1.f, 0.f, 0.f)
		},
		{
			Vector3f(-halfSize, -halfSize, 0.f),
			Vector2f(1.f, 0.f),
			Vector3f(0.f, 0.f, 1.f),
			Vector3f(-1.f, 0.f, 0.f)
		},
	};
	std::vector<UINT> mdlIndices{
		//0,1,2,
		//3,2,1
		1,2,3,
		2,1,0
	};
};

struct TransformGizmoData
{
	std::vector<Vertex> mdlVertices;
	std::vector<UINT> mdlIndices;

	TransformGizmoData()
	{
		const float axisLength = 100.f;
		const float shaftSize = 2.5f;
		const float headSize = 10.f;

		auto AddBox = [&](Vector3f center, Vector3f size)
			{
				int startIndex = (int)mdlVertices.size();

				float hx = size.x * 0.5f;
				float hy = size.y * 0.5f;
				float hz = size.z * 0.5f;

				Vector3f v[8] =
				{
					center + Vector3f(-hx,-hy,-hz),
					center + Vector3f(hx,-hy,-hz),
					center + Vector3f(hx, hy,-hz),
					center + Vector3f(-hx, hy,-hz),

					center + Vector3f(-hx,-hy, hz),
					center + Vector3f(hx,-hy, hz),
					center + Vector3f(hx, hy, hz),
					center + Vector3f(-hx, hy, hz),
				};

				for (int i = 0; i < 8; i++)
				{
					mdlVertices.push_back(
						{
							v[i],
							Vector2f(0,0),
							Vector3f(0,1,0),
							Vector3f(1,0,0)
						});
				}

				UINT inds[] =
				{
					0,1,2, 0,2,3,
					4,6,5, 4,7,6,
					0,4,5, 0,5,1,
					1,5,6, 1,6,2,
					2,6,7, 2,7,3,
					3,7,4, 3,4,0
				};

				for (UINT i : inds)
					mdlIndices.push_back(startIndex + i);
			};

		auto AddPyramid = [&](Vector3f baseCenter, Vector3f dir)
			{
				int startIndex = (int)mdlVertices.size();

				Vector3f up = dir * headSize;
				Vector3f right;

				if (fabs(dir.y) > 0.5f)
					right = Vector3f(1, 0, 0);
				else
					right = Vector3f(0, 1, 0);

				Vector3f forward = dir.Cross(right);
				right = forward.Cross(dir);

				right.Normalize();
				forward.Normalize();

				right *= headSize * 0.5f;
				forward *= headSize * 0.5f;

				Vector3f tip = baseCenter + up;

				Vector3f base[4] =
				{
					baseCenter + right + forward,
					baseCenter - right + forward,
					baseCenter - right - forward,
					baseCenter + right - forward
				};

				mdlVertices.push_back({ tip, {}, dir, right });
				for (int i = 0; i < 4; i++)
					mdlVertices.push_back({ base[i], {}, dir, right });

				UINT inds[] =
				{
					0,1,2,
					0,2,3,
					0,3,4,
					0,4,1,
					1,4,3,
					1,3,2
				};

				for (UINT i : inds)
					mdlIndices.push_back(startIndex + i);
			};

		// X axis
		AddBox(
			Vector3f(axisLength * 0.5f, 0, 0),
			Vector3f(axisLength, shaftSize, shaftSize)
		);

		AddPyramid(
			Vector3f(axisLength, 0, 0),
			Vector3f(1, 0, 0)
		);

		// Y axis
		AddBox(
			Vector3f(0, axisLength * 0.5f, 0),
			Vector3f(shaftSize, axisLength, shaftSize)
		);

		AddPyramid(
			Vector3f(0, axisLength, 0),
			Vector3f(0, 1, 0)
		);

		// Z axis
		AddBox(
			Vector3f(0, 0, axisLength * 0.5f),
			Vector3f(shaftSize, shaftSize, axisLength)
		);

		AddPyramid(
			Vector3f(0, 0, axisLength),
			Vector3f(0, 0, 1)
		);
	}
};
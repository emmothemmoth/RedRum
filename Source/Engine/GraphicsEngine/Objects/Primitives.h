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
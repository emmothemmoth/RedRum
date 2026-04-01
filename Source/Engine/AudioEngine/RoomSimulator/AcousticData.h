#pragma once

#include "CommonUtilities/Vector3.hpp"
#include "CommonUtilities/Matrix.hpp"

struct GPURay
{
	CU::Vector3f Origin;
	float Power;
	CU::Vector3f Direction;
	float Padding; 
};

struct GPUObstacle
{
	CU::Matrix4x4f InverseTransform; // 64 bytes
	CU::Matrix4x4f Transform;        // 64 bytes
	CU::Vector3f MinPoint;
	float Absorption; 
	CU::Vector3f MaxPoint;
	float Padding;      
};

struct GPURayResult
{
	int HitListener; 
	float TotalDistance;
	float FinalPower;
	float PanAngle; 
};

struct AcousticSceneData
{
	CU::Vector3f ListenerPos;
	float ListenerRadius;
	CU::Vector3f ListenerRight;
	int ObstacleCount;
};

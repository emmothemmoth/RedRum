#pragma once

#include "CommonUtilities/Vector3.hpp"
#include "CommonUtilities/Vector2.hpp"
#include "CommonUtilities/Matrix.hpp"

struct GPURay
{
	CU::Vector3f Origin;
	float Padding1;
	CU::Vector3f Power;
	float Padding2;
	CU::Vector3f Direction;
	float Padding3; 
};

struct GPUObstacle
{
	CU::Matrix4x4f InverseTransform; // 64 bytes
	CU::Matrix4x4f Transform;        // 64 bytes
	CU::Vector3f MinPoint;
	float Padding1; 
	CU::Vector3f MaxPoint;
	float Padding2;
	CU::Vector3f Reflection;
	float Padding3;
};

struct GPURayResult
{
	CU::Vector3f RayDirection;
	int HitListener; 
	float TotalDistance;
	float FinalPower;
	CU::Vector2f Padding;
};

struct AcousticSceneData
{
	CU::Vector3f ListenerPos;    // 12 bytes
	float ListenerRadius;        // 4 bytes  (Total: 16)
	CU::Vector3f ListenerRight;  // 12 bytes
	int ObstacleCount;           // 4 bytes  (Total: 32)
	int TotalProbeCount;         // 4 bytes
	CU::Vector3f PaddingCB;      // 12 bytes (Total: 48) -> PERFECT! Multiple of 16.
};

struct AcousticHit
{
	float delay; 
	float gainL;
	float gainR;
	int bounceCount; 
};

struct GPUMegaHit
{
	CU::Vector3f RayDirection;
	int ProbeIndex;
	CU::Vector3f Power;
	float Distance;
};

struct AcousticProbe
{
	CU::Vector3f Position;
	std::vector<GPUMegaHit> Hits;
};

#pragma once

#include "CommonUtilities\Vector.hpp"
#include "CommonUtilities\Curve.h"

#include <unordered_map>
#include <memory>

class TextureAsset;

enum class EmitterShape
{
	None,
	Box,
	Sphere,
};

struct ParticleEmitterSettings
{
	EmitterShape Shape = EmitterShape::None;
	float ShapeRadius = 0.0f;
	CU::Vector3f EmitterSize = { 1.0f, 1.0f, 1.0f};
	float SpawnRate = 1.0f;
	CU::Curve<CU::Vector4<float>> Color;
	CU::Curve<CU::Vector2<float>> Size;
	CU::Vector3f Velocity = { 0.0f, 0.0f, 0.0f };
	float LifeTime = 5.0f;
	float GravityScale = 1.0;
	CU::Curve<float> Angle;
	CU::Vector4f ChannelMask = { 1.0f, 0.0f, 0.0f, 0.0f };

	std::unordered_map<unsigned, std::shared_ptr<TextureAsset>> Textures;

};


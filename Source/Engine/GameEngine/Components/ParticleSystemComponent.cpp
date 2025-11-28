#include "GameEngine.pch.h"
#include "ParticleSystemComponent.h"

#include "../GraphicsEngine/Objects/ParticleSystem.h"
#include "../GraphicsEngine/Objects/ParticleEmitterSettings.h"
#include "../GraphicsEngine/Objects/StarParticleEmitter.h"
#include "../GraphicsEngine/Objects/OtherParticleEmitter.h"

#include <fstream>

#include "../AssetManager/AssetManager.h"
#include "..\External\nlohmann\json.hpp"


ParticleSystemComponent::ParticleSystemComponent(GameObject& aParent)
	:Component(aParent)
{
	myParticleSystem = std::make_shared<ParticleSystem>();
}

ParticleSystemComponent::~ParticleSystemComponent()
{
}

void ParticleSystemComponent::Init(ParticleEmitterType aType)
{
	ParticleEmitterSettings settings;
	settings.Angle.AddKey(0.5f, 0.9f);
	settings.ChannelMask = { 1.0f, 1.0f, 1.0f, 1.0f };
	settings.Color.AddKey(0.0f, CU::Vector4f{ 0.0f, 0.0f, 0.0f, 1.0f });
	settings.Color.AddKey(0.0f, CU::Vector4f{ 0.5f, 0.0f, 0.0f, 1.0f });
	settings.Color.AddKey(0.0f, CU::Vector4f{ 1.f, 0.0f, 0.0f, 1.0f });
	settings.EmitterSize = { 100.0f, 100.0f, 100.0f };
	settings.SpawnRate = 20.0f;
	switch (aType)
	{
	case ParticleEmitterType::General:
		settings.Textures.insert({ 5, AssetManager::Get().GetAsset<TextureAsset>("T_ParticleStar") });
		settings.Shape = EmitterShape::Sphere;
		settings.ShapeRadius = 200.0f;
		settings.LifeTime = 20.0f;
		break;
	case ParticleEmitterType::StarParticle:
		settings.Textures.insert({ 5, AssetManager::Get().GetAsset<TextureAsset>("T_ParticleStar") });
		settings.Shape = EmitterShape::Sphere;
		settings.ShapeRadius = 100.0f;
		settings.LifeTime = 10.0f;
		break;
	case ParticleEmitterType::Other:
		settings.Textures.insert({ 5, AssetManager::Get().GetAsset<TextureAsset>("T_ParticleStar") });
		settings.Shape = EmitterShape::Box;
		settings.ShapeRadius = 200.0f;
		settings.LifeTime = 20.0f;
		break;
	default:
		break;
	}
	settings.GravityScale = 2.0f;
	settings.Velocity = { 20.f, 20.f, 20.0f };
	switch (aType)
	{
	case ParticleEmitterType::General:
		myParticleSystem->AddEmitter<ParticleEmitter>(settings);
		break;
	case ParticleEmitterType::StarParticle:
		myParticleSystem->AddEmitter<StarParticleEmitter>(settings);
		break;
	case ParticleEmitterType::Other:
		myParticleSystem->AddEmitter<OtherParticleEmitter>(settings);
		break;
	default:
		break;
	}
}

bool ParticleSystemComponent::Init(const std::filesystem::path& aFileNameWithExtension)
{
	if (!aFileNameWithExtension.has_extension() || !aFileNameWithExtension.has_filename() || aFileNameWithExtension.empty())
	{
		return false;
	}
	std::filesystem::path path = "Content\\Particles\\" / aFileNameWithExtension;
	InitFromJSON(path);
	return true;
}

void ParticleSystemComponent::Update(const float aDeltaTime)
{
	myParticleSystem->Update(aDeltaTime);
}

void ParticleSystemComponent::Render()
{

}

std::shared_ptr<ParticleSystem> ParticleSystemComponent::GetParticleSystem() const
{
	return myParticleSystem;
}

bool ParticleSystemComponent::InitFromJSON(const std::filesystem::path& aPath)
{
	std::string contentPath = aPath.string();
	nlohmann::json jsonReader;
	std::fstream file(contentPath);
	assert(file.good() && "file not found");

	if (!file.good())
	{
		return false;
	}
	file >> jsonReader;

	ParticleEmitterSettings settings;
	if (jsonReader.contains("Shape"))
	{
		std::string shape = jsonReader["Shape"].get<std::string>();
		if (shape == "Box" || shape == "box")
		{
			settings.Shape = EmitterShape::Box;
		}
		else if (shape == "Sphere" || shape == "sphere")
		{
			settings.Shape = EmitterShape::Sphere;
		}
		else
		{
			settings.Shape = EmitterShape::None;
		}
	}
	if (jsonReader.contains("ShapeRadius"))
	{
		settings.ShapeRadius = jsonReader["ShapeRadius"];
	}
	if (jsonReader.contains("AngleKeys"))
	{
		for (auto& key : jsonReader["AngleKeys"])
		{
			if (key.contains("Position") && key.contains("Value"))
			{
				settings.Angle.AddKey(key["Position"], key["Value"]);
			}
		}
	}
	if (jsonReader.contains("ChannelMask"))
	{
		CU::Vector4f channelMask = { 1.0f, 1.0f, 1.0f, 1.0f };
		if (jsonReader["ChannelMask"].contains("R"))
		{
			channelMask.x = jsonReader["ChannelMask"];
		}
		if (jsonReader["ChannelMask"].contains("G"))
		{
			channelMask.y = jsonReader["ChannelMask"];
		}
		if (jsonReader["ChannelMask"].contains("B"))
		{
			channelMask.z = jsonReader["ChannelMask"];
		}
		if (jsonReader["ChannelMask"].contains("A"))
		{
			channelMask.w = jsonReader["ChannelMask"];
		}
		settings.ChannelMask = channelMask;
	}
	if (jsonReader.contains("Color"))
	{
		for (auto& colorKey : jsonReader["Color"])
		{

			CU::Vector4f color = { 1.0f, 1.0f, 1.0f, 1.0f };
			float position = 0.0f;
			if (colorKey.contains("R"))
			{
				position = colorKey["Position"];
			}
			if (jsonReader["Color"].contains("R"))
			{
				color.x = jsonReader["Color"]["R"];
			}
			if (jsonReader["Color"].contains("G"))
			{
				color.y = jsonReader["Color"]["G"];
			}
			if (jsonReader["Color"].contains("B"))
			{
				color.z = jsonReader["Color"]["B"];
			}
			if (jsonReader["Color"].contains("A"))
			{
				color.w = jsonReader["ChannelMask"]["A"];
			}
			settings.Color.AddKey(position, color);
		}
	}
	if (jsonReader.contains("EmitterSize"))
	{
		for (auto& key : jsonReader["EmitterSize"])
		{
			if (key.contains("Position") && key.contains("SizeX") && key.contains("SizeY"))
			{
				CU::Vector2f size = { key["SizeX"], key["SizeY"]};
				settings.Size.AddKey(key["Position"], size);
			}
		}
	}
	if (jsonReader.contains("LifeTime"))
	{
		settings.LifeTime = jsonReader["LifeTime"];
	}
	if (jsonReader.contains("Textures"))
	{
		for (auto& texture : jsonReader["Textures"])
		{
			if (texture.contains("Slot") && texture.contains("Texture"))
			{
				std::filesystem::path texturePath = texture["Texture"];
				std::shared_ptr<TextureAsset> textureAsset = AssetManager::Get().GetAsset<TextureAsset>(texturePath);
				settings.Textures.insert({ texture["Slot"], textureAsset });
			}
		}
	}
	if (jsonReader.contains("GravityScale"))
	{
		settings.GravityScale = jsonReader["GravityScale"];
	}
	if (jsonReader.contains("Velocity"))
	{
		if (jsonReader["Velocity"].contains("X") && jsonReader["Velocity"].contains("Y") && jsonReader["Velocity"].contains("Z"))
		{
			CU::Vector3f velocity = { 1.0f, 1.0f , 1.0f };
			velocity.x = jsonReader["Velocity"]["X"];
			velocity.y = jsonReader["Velocity"]["Y"];
			velocity.z = jsonReader["Velocity"]["Z"];
			settings.Velocity = velocity;
		}
	}
	ParticleEmitterType emitterType = ParticleEmitterType::General;
	if (jsonReader.contains("Type"))
	{
		std::string type = jsonReader["Type"];

		if (type == "Other" || type == "other")
		{
			emitterType = ParticleEmitterType::Other;
		}
		else if (type == "Star" || type == "star")
		{
			emitterType = ParticleEmitterType::StarParticle;
		}
	}
	switch (emitterType)
	{
	case ParticleEmitterType::General:
		myParticleSystem->AddEmitter<ParticleEmitter>(settings);
		break;
	case ParticleEmitterType::StarParticle:
		myParticleSystem->AddEmitter<StarParticleEmitter>(settings);
		break;
	case ParticleEmitterType::Other:
		myParticleSystem->AddEmitter<OtherParticleEmitter>(settings);
		break;
	default:
		break;
	}
	
	return true;
}

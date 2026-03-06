#include "GameEngine.pch.h"
#include "LevelLoader.h"
#include "Level.h"
#include "Components/ComponentsInclude.h"
#include "Components/ComponentType.h"
#include "../GraphicsEngine/Objects/MeshAsset.h"
#include "../GraphicsEngine/Objects/AnimationAsset.h"
#include "../GraphicsEngine/Objects/MaterialAsset.h"

#include "..\AssetManager\AssetManager.h"

#include "../External/nlohmann/json.hpp"

#include <fstream>

bool LevelLoader::LoadLevelFromJSON(const std::filesystem::path& aLevelPath, Level& inOutLevel)
{
    nlohmann::json jsonReader;
    std::fstream file(aLevelPath);
    if (!file.good())
    {
        return false;
    }
    file >> jsonReader;
    assert(jsonReader.contains("Entities") && "Level without entities detected!");

    unsigned gameObjectIDCounter = 0;
    for (auto& entity : jsonReader["Entities"])
    {
        std::shared_ptr<GameObject> gameObject;
        std::string name = entity.at("Name");
        float rotationX, rotationY, rotationZ;
        if (name.starts_with("Camera"))
        {
            inOutLevel.Camera = std::make_shared<GameObject>(name, gameObjectIDCounter++);
            gameObject = inOutLevel.Camera;
        }
        else
        {
            inOutLevel.GameObjects.emplace_back(std::make_shared<GameObject>(name, gameObjectIDCounter++));
            gameObject = inOutLevel.GameObjects.back();
        }
   
        {
            nlohmann::json transform = entity.at("Transform");
            gameObject->SetPosition(transform.at("Position")[0], transform.at("Position")[1], transform.at("Position")[2]);
            rotationX = transform.at("RotationDegrees")[0];
            rotationY = transform.at("RotationDegrees")[1];
            rotationZ = transform.at("RotationDegrees")[2];
            gameObject->RotateAroundX(rotationX);
            gameObject->RotateAroundY(rotationY);
            gameObject->RotateAroundX(rotationZ);
        }
        {
            for (auto& component : entity.at("Components"))
            {
                unsigned componentType = component.at("ComponentTypeID");
                switch (static_cast<ComponentType>(componentType))
                {
                case ComponentType::Mesh:
                {
                    gameObject->AddComponent(std::make_shared<MeshComponent>(*gameObject, AssetManager::Get().GetAsset<MeshAsset>(component.at("Mesh"))));
                    if (component.contains("RenderPass"))
                    {
                        std::string renderPass = component.at("RenderPass");
                        if (renderPass.starts_with("Forward"))
                        {
                            gameObject->GetLastAddedComponent<MeshComponent>()->SetRenderStage(RenderStage::Forward);
                            gameObject->GetLastAddedComponent<MeshComponent>()->SetRenderStage(RenderStage::Deferred, false);
                        }
                    }
                    if (component.contains("Material"))
                    {
                        std::string material = component.at("Material");
                        if (!material.empty())
                        {
                            gameObject->GetLastAddedComponent<MeshComponent>()->AddMaterial(AssetManager::Get().GetAsset<MaterialAsset>(material));
                        }
                    }
                    break;
                }
                case ComponentType::MeshInstance:
                {
                    gameObject->AddComponent(std::make_shared<MeshInstancedComponent>(*gameObject, AssetManager::Get().GetAsset<MeshAsset>(component.at("Mesh"))));
                    if (component.contains("RenderPass"))
                    {
                        std::string renderPass = component.at("RenderPass");
                        if (renderPass.starts_with("forward"))
                        {
                            gameObject->GetLastAddedComponent<MeshComponent>()->SetRenderStage(RenderStage::Forward);
                            gameObject->GetLastAddedComponent<MeshComponent>()->SetRenderStage(RenderStage::Deferred, false);
                        }
                    }
                    if (component.contains("Material"))
                    {
                        std::string material = component.at("Material");
                        if (!material.empty())
                        {
                            gameObject->GetLastAddedComponent<MeshInstancedComponent>()->AddMaterial(AssetManager::Get().GetAsset<MaterialAsset>(material));
                        }
                    }
                    for (auto& instance : component.at("Instances"))
                    {
                        CU::Matrix4x4f instanceTransform;
                        CU::Vector4f translation;
                        translation.x = instance.at("Position")[0];
                        translation.y = instance.at("Position")[1];
                        translation.z = instance.at("Position")[2];
                        translation.w = 1.0f;
                        CU::Matrix4x4f matrix;
                        matrix = CU::Matrix4x4f::CreateRotationAroundX(instance.at("RotationDegrees")[0]);
                        matrix = matrix * CU::Matrix4x4f::CreateRotationAroundY(instance.at("RotationDegrees")[1]);
                        matrix = matrix * CU::Matrix4x4f::CreateRotationAroundZ(instance.at("RotationDegrees")[2]);
                        matrix.SetRow(translation, 4);
                        gameObject->GetComponent<MeshInstancedComponent>()->AddInstance(matrix);
                    }
                    gameObject->GetComponent<MeshInstancedComponent>()->Init();
                    break;
                }
                case ComponentType::Animation:
                    gameObject->AddComponent(std::make_shared<AnimationComponent>(*gameObject, gameObject->GetLastAddedComponent<MeshComponent>()->GetMesh()->GetSkeleton()));
                    for (auto& animation : component.at("Animations"))
                    {
                        unsigned stateID = animation.at("StateID");
                        gameObject->GetComponent<AnimationComponent>()->AddAnimation(static_cast<AnimationState>(stateID), AssetManager::Get().GetAsset<AnimationAsset>(animation.at("Animation")));
                    }
                    gameObject->GetComponent<AnimationComponent>()->SetAnimationState(AnimationState::Idle);
                    break;
                case ComponentType::Controller:
                    gameObject->AddComponent(std::make_shared<ControllerComponent>(*gameObject));
                    break;
                case ComponentType::ParticleSystem:
                {
                    unsigned emitterID = component.at("EmitterID");
                    gameObject->AddComponent(std::make_shared<ParticleSystemComponent>(*gameObject));
                    gameObject->GetComponent<ParticleSystemComponent>()->Init(static_cast<ParticleEmitterType>(emitterID));
                    break;
                }
                case ComponentType::Camera:
                    gameObject->AddComponent(std::make_shared<CameraComponent>(*gameObject));
                    gameObject->GetComponent<CameraComponent>()->Init(gameObject->GetPosition(), {rotationX, rotationY, rotationZ});
                    break;
                case ComponentType::AudioSource:
                    gameObject->AddComponent(std::make_shared<AudioSourceComponent>(*gameObject));
                    gameObject->GetComponent<AudioSourceComponent>()->Init(component.at("File"));
                    break;
                default:
                    break;
                }
            }
        }


    }
    return true;
}

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

bool LevelLoader::LoadLevelFromJSON(const std::filesystem::path& aLevelPath, Level& inOutLevel, uint32_t& outIDCount, uint32_t& outListenerID)
{
    nlohmann::json jsonReader;
    std::fstream file(aLevelPath);
    if (!file.good())
    {
        return false;
    }
    file >> jsonReader;
    assert(jsonReader.contains("Entities") && "Level without entities detected!");

    uint32_t gameObjectIDCounter = 0;
    for (auto& entity : jsonReader["Entities"])
    {
        std::shared_ptr<GameObject> gameObject;
        std::string name = entity.at("Name");
        bool isListener = false;
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
        isListener = name.starts_with("Listener");
   
        {
            nlohmann::json transform = entity.at("Transform");
            gameObject->SetPosition(transform.at("Position")[0], transform.at("Position")[1], transform.at("Position")[2]);
            rotationX = transform.at("RotationDegrees")[0];
            rotationY = transform.at("RotationDegrees")[1];
            rotationZ = transform.at("RotationDegrees")[2];
            gameObject->SetRotation({ rotationX, rotationY, rotationZ });
            gameObject->SetScale({ transform.at("scale")[0], transform.at("scale")[1], transform.at("scale")[2] });
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
                    std::shared_ptr<MeshComponent> meshComp = gameObject->GetLastAddedComponent<MeshComponent>();
                    if (component.contains("RenderPass"))
                    {
                        std::string renderPass = component.at("RenderPass");
                        if (renderPass.starts_with("Forward"))
                        {
                            meshComp->SetRenderStage(RenderStage::Forward);
                            meshComp->SetRenderStage(RenderStage::Deferred, false);
                        }
                    }
                    if (component.contains("Material"))
                    {
                        std::string material = component.at("Material");
                        if (!material.empty())
                        {
                            meshComp->AddMaterial(AssetManager::Get().GetAsset<MaterialAsset>(material));
                        }
                    }
                   //CU::Vector4f iconOffset = { 0.0f, meshComp->GetMesh()->GetMaxPoint().y + 50.0f, 0.0f, 0.0f };
                    gameObject->SetIcon(ComponentType::Mesh/*, iconOffset*/);
                    break;
                }
                case ComponentType::MeshInstance:
                {
                    gameObject->AddComponent(std::make_shared<MeshInstancedComponent>(*gameObject, AssetManager::Get().GetAsset<MeshAsset>(component.at("Mesh"))));
                    std::shared_ptr<MeshInstancedComponent> meshInstancedComp = gameObject->GetLastAddedComponent<MeshInstancedComponent>();
                    if (component.contains("RenderPass"))
                    {
                        std::string renderPass = component.at("RenderPass");
                        if (renderPass.starts_with("forward"))
                        {
                            meshInstancedComp->SetRenderStage(RenderStage::Forward);
                            meshInstancedComp->SetRenderStage(RenderStage::Deferred, false);
                        }
                    }
                    if (component.contains("Material"))
                    {
                        std::string material = component.at("Material");
                        if (!material.empty())
                        {
                            meshInstancedComp->AddMaterial(AssetManager::Get().GetAsset<MaterialAsset>(material));
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
                        meshInstancedComp->AddInstance(matrix);
                    }
                    gameObject->GetComponent<MeshInstancedComponent>()->Init();
                    //CU::Vector4f iconOffset = { 0.0f, meshInstancedComp->GetMesh()->GetMaxPoint().y + 50.0f, 0.0f, 0.0f };
                    gameObject->SetIcon(ComponentType::MeshInstance/*, iconOffset*/);
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
                    if (isListener)
                    {
                        gameObject->GetComponent<CameraComponent>()->SetVisible(false);
                        gameObject->GetComponent<CameraComponent>()->SetEnabled(false);
                    }
                    break;
                case ComponentType::AudioSource:
                    {
                        gameObject->AddComponent(std::make_shared<AudioSourceComponent>(*gameObject));
                        if (component.contains("File"))
                        {
                            std::string audioFile = component.at("File");
                            gameObject->GetComponent<AudioSourceComponent>()->Init(audioFile);
                        }
                        gameObject->SetIcon(ComponentType::AudioSource, {0.0f, 60.0f, 0.0f, 0.0f});
                    }
                    break;
                case ComponentType::Listener:
                {
                    outListenerID = gameObject->GetID();
                    gameObject->AddComponent(std::make_shared<ListenerComponent>(*gameObject));
                    gameObject->SetIcon(ComponentType::Listener, { 0.0f, 125.0f, 0.0f, 0.0f });
                }
                break;
                default:
                    break;
                }
            }
        }


    }

    //Find ground
    for (auto& object : inOutLevel.GameObjects)
    {
        if (object->GetName().find("Ground") != std::string::npos
            ||
            object->GetName().find("ground") != std::string::npos)
        {
            std::shared_ptr<BillboardComponent> billboard = object->GetComponent<BillboardComponent>();
            if (billboard)
            {
                billboard->SetVisible(false);
            }
        }
    }
    outIDCount = gameObjectIDCounter;
    return true;
}

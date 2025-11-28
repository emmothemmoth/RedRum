#include "GameEngine.pch.h"
#include "LevelLoader.h"
#include "Level.h"
#include "Components/ComponentsInclude.h"
#include "../GraphicsEngine/Objects/MeshAsset.h"

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

    for (auto& entity : jsonReader["Entities"])
    {
        std::shared_ptr<GameObject> gameObject;
        std::string name = entity.at("Name");
        std::string renderPass;
        float rotationX, rotationY, rotationZ;
        if (jsonReader.contains("RenderPass"))
        {
            renderPass = entity.at("RenderPass");
        }
        unsigned char ID = entity.at("ID");
        if (ID == 0)
        {
            inOutLevel.Camera = std::make_shared<GameObject>(name, ID);
            gameObject = inOutLevel.Camera;
        }
        else
        {
            if (renderPass == "Forward")
            {
                inOutLevel.ForwardObjects.emplace_back(std::make_shared<GameObject>(name, ID));
                gameObject = inOutLevel.ForwardObjects.back();
            }
            else
            {
                inOutLevel.DeferredObjects.emplace_back(std::make_shared<GameObject>(name, ID));
                gameObject = inOutLevel.DeferredObjects.back();
            }
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
                unsigned componentID = component.at("ComponentID");
                unsigned emitterID;
                switch (componentID)
                {
                case 0:
                    gameObject->AddComponent(std::make_shared<MeshComponent>(*gameObject, AssetManager::Get().GetAsset<MeshAsset>(component.at("Mesh"))));
                    break;
                case 1:
                    gameObject->AddComponent(std::make_shared<MeshInstancedComponent>(*gameObject, AssetManager::Get().GetAsset<MeshAsset>(component.at("Mesh"))));
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
                    break;
                case 2:
                    gameObject->AddComponent(std::make_shared<MaterialComponent>(*gameObject, AssetManager::Get().GetAsset<MaterialAsset>(component.at("Material"))));
                    break;
                case 3:
                    gameObject->AddComponent(std::make_shared<AnimationComponent>(*gameObject, gameObject->GetComponent<MeshComponent>()->GetMesh()->GetSkeleton()));
                    for (auto& animation : component.at("Animations"))
                    {
                        unsigned stateID = animation.at("StateID");
                        gameObject->GetComponent<AnimationComponent>()->AddAnimation(static_cast<AnimationState>(stateID), AssetManager::Get().GetAsset<AnimationAsset>(animation.at("Animation")));
                    }
                    gameObject->GetComponent<AnimationComponent>()->SetAnimationState(AnimationState::Idle);
                    break;
                case 4:
                    gameObject->AddComponent(std::make_shared<ControllerComponent>(*gameObject));
                    break;
                case 5:
                    emitterID = component.at("EmitterID");
                    gameObject->AddComponent(std::make_shared<ParticleSystemComponent>(*gameObject));
                    gameObject->GetComponent<ParticleSystemComponent>()->Init(static_cast<ParticleEmitterType>(emitterID));
                    break;
                case 6:
                    gameObject->AddComponent(std::make_shared<CameraComponent>(*gameObject));
                    gameObject->GetComponent<CameraComponent>()->Init(gameObject->GetPosition(), {rotationX, rotationY, rotationZ});
                    break;
                default:
                    break;
                }
            }
        }


    }
    return true;
}

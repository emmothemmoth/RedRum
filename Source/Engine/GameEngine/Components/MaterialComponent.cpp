#include "GameEngine.pch.h"
#include "MaterialComponent.h"

#include "GameObject.h"

#include "..\GraphicsEngine\Objects\MaterialAsset.h"

#include "../../AssetManager/AssetManager.h"


MaterialComponent::MaterialComponent(GameObject& aParent, std::shared_ptr<MaterialAsset> aMaterial)
    : Component(aParent)
{
    myID = ComponentID::Material;
   // myMaterial = aMaterial;
    myMaterials.push_back(aMaterial);
}

MaterialComponent::~MaterialComponent()
{
}

void MaterialComponent::AddMaterial(std::shared_ptr<MaterialAsset> aMaterial)
{
    myMaterials.push_back(aMaterial);
}

std::vector<std::shared_ptr<MaterialAsset>> MaterialComponent::GetMaterials()
{
    return myMaterials;
}

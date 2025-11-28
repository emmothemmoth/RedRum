#pragma once
#include "Component.h"

#include <vector>


class MaterialAsset;

class MaterialComponent : public Component
{
public:
	MaterialComponent() = delete;
	//MaterialComponent(GameObject& aParent, std::weak_ptr<MaterialAsset> aMaterial);
	MaterialComponent(GameObject& aParent, std::shared_ptr<MaterialAsset> aMaterial);
	~MaterialComponent();

	void AddMaterial(std::shared_ptr<MaterialAsset> aMaterial);
	//std::weak_ptr<MaterialAsset> GetMaterial();
	std::vector<std::shared_ptr<MaterialAsset>> GetMaterials();
	
private:
	//std::weak_ptr<MaterialAsset> myMaterial;
	//std::shared_ptr<MaterialAsset> myMaterial;
	std::vector<std::shared_ptr<MaterialAsset>> myMaterials;
};

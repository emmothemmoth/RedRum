#pragma once

#include "Level.h"
#include "LevelLoader.h"
#include "Events/DelegateHandle.h"

#include "../Utilities/CommonUtilities\Timer.h"
#include "../Utilities/CommonUtilities\InputObserver.h"

#include "..\GraphicsEngine/Buffers/LightBuffer.h"

#include <unordered_map>
#include <filesystem>

class LightManager;
class IEntity;
class Component;
class CameraComponent;
class ShadowCameraComponent;
class AnimationComponent;
class GameObject;
struct DebugBuffer;

struct SortingInfo
{
	float CameraDistance;
	std::shared_ptr<GameObject> Object;
};

class Scene : public InputObserver
{
public:
	Scene();
	~Scene();

	void Init();

	void LoadScene(const std::filesystem::path& aPath, bool aIsNetworkLevel = false);

	void InitSortingLists();

	void ClearScene();

	void UpdateScene(const float aDeltaTime);

	void RenderScene();

	void PresentScene();

	void CloseScene();

	void RecieveEvent(const ActionEvent& anEvent) override;

	Level& GetCurrentLevel();

	std::vector<std::shared_ptr<GameObject>>& GetGameObjects() { return myCurrentLevel.GameObjects; }

	std::shared_ptr<GameObject>& GetObjectByID(unsigned char anID);

private:
	void SortObjects();
	void InitSceneLights();
	void UpdateLights();
	void UpdateDebugInfo();
	void ResetScene(bool aShouldReload = true);

	void ObjectClicked(uint32_t anID);
	void SelectObject(GameObject& anObject);
	void MoveObject(GameObject& anObject, Gizmo_Axis anAxis);
	CU::Vector3f GetIntersection(GameObject& anObject, const CU::Vector3f& anAxis);

private:

	WorldBounds myWorldBounds;

	std::vector<SortingInfo> mySortingList;

	std::shared_ptr<GameObject> myDirLight;
	std::vector < std::shared_ptr<GameObject>> myPointLights;
	std::vector < std::shared_ptr<GameObject>> mySpotLights;
	std::shared_ptr<LightBuffer> myLightBuffer;

	std::shared_ptr<DebugBuffer> myDebugBuffer;

	LevelLoader myLevelLoader;
	Level myCurrentLevel;
	std::unordered_map<unsigned, unsigned> myIDtoIndex;

	std::string myCurrentScene;
	bool myShouldClear = false;

	DelegateHandle mySelectionHandle;

	CU::Vector3f myPreviousIntersection;
	Gizmo_Axis myCurrentAxisMovement = Gizmo_Axis::None;
	bool myIsMovingObject = false;
};

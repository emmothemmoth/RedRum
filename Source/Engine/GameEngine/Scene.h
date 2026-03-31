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
class ListenerComponent;
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

	void AddObject(std::shared_ptr<GameObject> anObject);

	void RemoveObject(const uint32_t anID);

	void InitSortingLists();

	void ClearScene();

	void UpdateScene(const float aDeltaTime);

	void RenderScene();

	void PresentScene();

	void CloseScene();

	void RecieveEvent(const ActionEvent& anEvent) override;

	Level& GetCurrentLevel();

	std::vector<std::shared_ptr<GameObject>>& GetGameObjects() { return myCurrentLevel.GameObjects; }

	std::shared_ptr<GameObject>& GetObjectByID(uint32_t anID);

	std::shared_ptr<GameObject> GetActiveCamera() const { return myActiveCamera; };
	void ChangeCamera(std::shared_ptr<GameObject> aCameraObject);
	void ResetCamera();

	std::shared_ptr<ListenerComponent> GetListener() const;

private:
	void SortObjects();
	void InitSceneLights();
	void UpdateLights();
	void UpdateDebugInfo();
	void ResetScene(bool aShouldReload = true);

private:

	WorldBounds myWorldBounds;

	std::vector<SortingInfo> mySortingList;
	 
	std::shared_ptr<GameObject> myActiveCamera;
	std::shared_ptr<GameObject> myDirLight;
	std::vector < std::shared_ptr<GameObject>> myPointLights;
	std::vector < std::shared_ptr<GameObject>> mySpotLights;
	std::shared_ptr<LightBuffer> myLightBuffer;

	std::shared_ptr<DebugBuffer> myDebugBuffer;

	LevelLoader myLevelLoader;
	Level myCurrentLevel;
	std::unordered_map<uint32_t, uint32_t> myIDtoIndex;
	uint32_t myIDCounter = 0;
	std::string myCurrentScene;
	bool myShouldClear = false;
	uint32_t myListenerID = UINT32_MAX;
};

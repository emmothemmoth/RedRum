#pragma once

#include "Level.h"
#include "LevelLoader.h"

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

	void LoadScene();

	void LoadScene(const std::filesystem::path& aPath, bool aIsNetworkLevel = false);

	void InitSortingLists();
	//MV
	void AddToScene(const std::filesystem::path& aPath);

	bool AddPlayer(const CU::Vector3f& aSpawnPosition, unsigned char anID, bool aIsUser = false);

	bool AddObject(const CU::Vector3f& aPosition, const char* aMesh, unsigned char anID, bool aIsActive = true, bool aIsUser = false);

	bool RemoveObjectByID(unsigned char anID);

	void ClearScene();

	void UpdateScene(const float aDeltaTime);

	void RenderScene();

	void PresentScene();

	void CloseScene();

	void RecieveEvent(const ActionEvent& anEvent) override;

	Level& GetCurrentLevel();

	std::vector<std::shared_ptr<GameObject>>& GetDeferredObjects() { return myCurrentLevel.DeferredObjects; }

	std::shared_ptr<GameObject>& GetObjectByID(unsigned char anID);

private:
	//Modelviewer
	void AddMeshToScene(const std::wstring& aFileName);
	void AddAnimationToMesh(const std::wstring& aFileName);
	void AddMaterialToMesh(const std::wstring& aFileName);
	void AddTextureToMesh(const std::wstring& aFileName);

	std::vector<SortingInfo> SortObjectsBackToFront(std::vector<std::shared_ptr<GameObject>> someObjects);
	std::vector<SortingInfo> SortObjectsFrontToBack(std::vector<std::shared_ptr<GameObject>> someObjects);
	void SortObjects();
	void InitSceneLights();
	void UpdateLights();
	void UpdateDebugInfo();
	void ResetScene(bool aShouldReload = true);


private:

	WorldBounds myWorldBounds;
	std::vector<std::shared_ptr<GameObject>> myDeferredObjects;
	std::vector<std::shared_ptr<GameObject>> myForwardObjects;

	std::vector<SortingInfo> myDeferredSort;
	std::vector<SortingInfo> myForwardSort;

	std::shared_ptr<GameObject> myDirLight;
	std::vector < std::shared_ptr<GameObject>> myPointLights;
	std::vector < std::shared_ptr<GameObject>> mySpotLights;
	std::shared_ptr<LightBuffer> myLightBuffer;

	std::shared_ptr<GameObject> myCamera;
	std::shared_ptr<GameObject> myShadowCamera;

	std::shared_ptr<DebugBuffer> myDebugBuffer;

	LevelLoader myLevelLoader;
	Level myCurrentLevel;
	std::unordered_map<unsigned char, unsigned> myIDtoIndex;
	std::unordered_map<unsigned char, unsigned char> myPlayerIDToObjectID;

	std::string myCurrentScene;
	bool myShouldClear = false;
};

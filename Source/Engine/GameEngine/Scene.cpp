#include "Scene.h"
#include "GameEngine.pch.h"
#include "Scene.h"
#include "Level.h"
#include "MainSingleton.h"
#include "Components\ComponentsInclude.h"

#include "CommonUtilities\InputMapper.h"

#include "..\AssetManager\AssetManager.h"

#include "..\GraphicsEngine\GraphicsEngine.h"
#include "..\GraphicsEngine\GraphicsCommands.h"
#include "..\GraphicsEngine\PipelineStates.h"
#include "../GraphicsEngine/Buffers/DebugBuffer.h"

#include "Logger/Logger.h"

#include <algorithm>

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(SceneLog, "SL", Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(SceneLog, "SL", Error);
#endif

DEFINE_LOG_CATEGORY(SceneLog);

typedef CommonUtilities::Vector3<float> Vector3f;

Scene::Scene() = default;

Scene::~Scene() = default;

void Scene::Init()
{
	CU::InputMapper::GetInputMapper().Register(ActionEventID::ChangeTonemap, this);
	CU::InputMapper::GetInputMapper().Register(ActionEventID::ChangeLuminanceMode, this);
	CU::InputMapper::GetInputMapper().Register(ActionEventID::ExposureUp, this);
	CU::InputMapper::GetInputMapper().Register(ActionEventID::ExposureDown, this);
	CU::InputMapper::GetInputMapper().Register(ActionEventID::Toggle_SSAO, this);

	myDebugBuffer = std::make_shared<DebugBuffer>();
	myDebugBuffer->BloomMode = 0;
	myDebugBuffer->LuminanceMode = 1;
	myDebugBuffer->Exposure = 1.0f;
	myDebugBuffer->Tonemap = 3;
	myDebugBuffer->SSAOActive = true;
}


void Scene::LoadScene()
{

	myWorldBounds.Radius = 1000.0f;
	myWorldBounds.Origin = Vector3f({ 0.0f, 0.0f, 0.0f });

	myLightBuffer = std::make_shared<LightBuffer>();
	myDirLight = std::make_shared<GameObject>("DirLight");
	myPointLights.reserve(4);
	mySpotLights.reserve(4);
	InitSceneLights();

	myCamera = std::make_shared<GameObject>("Camera");
	myCamera->AddComponent(std::make_shared<CameraComponent>(*myCamera));
	myCamera->GetComponent<CameraComponent>()->Init(CommonUtilities::Vector3<float>(0, 100, -1000));

	//myDeferredObjects.emplace_back(std::make_shared<GameObject>("Ground"));
	//myDeferredObjects.back()->AddComponent(std::make_shared<MeshComponent>(*myDeferredObjects.back(), AssetManager::Get().GetAsset<MeshAsset>(L"GroundMesh")));
	//myDeferredObjects.back()->SetPosition(0.0f, 0.0f, 0.0f);
	//myGroundIsVisible = true;
	//
	//myDeferredObjects.emplace_back(std::make_shared<GameObject>("Sphere"));
	//myDeferredObjects.back()->AddComponent(std::make_shared<MeshComponent>(*myDeferredObjects.back(), AssetManager::Get().GetAsset<MeshAsset>(L"SphereMesh")));
	//myDeferredObjects.back()->SetPosition(-500.0f, 50.0f, 300.0f);
	//
	//myDeferredObjects.emplace_back(std::make_shared<GameObject>("PlaneH"));
	//myDeferredObjects.back()->AddComponent(std::make_shared<MeshComponent>(*myDeferredObjects.back(), AssetManager::Get().GetAsset<MeshAsset>(L"HorizontalPlaneMesh")));
	//myDeferredObjects.back()->RotateAroundY(180.0f);
	//myDeferredObjects.back()->SetPosition(0.0f, 500.0f, 50.0f);
	//
	//myDeferredObjects.emplace_back(std::make_shared<GameObject>("Cube"));
	//myDeferredObjects.back()->AddComponent(std::make_shared<MeshInstancedComponent>(*myDeferredObjects.back(), AssetManager::Get().GetAsset<MeshAsset>(L"CubeMesh")));
	//for (int index = 0; index < 100; index++)
	//{
	//	CU::Matrix4x4f offset;
	//	CU::Vector4f translation = { 50.0f * static_cast<float>(index) - 950.0f , 50.0f,  50.0f * static_cast<float>(index) - 950.0f, 1.0f };
	//	offset.SetRow(translation, 4);
	//	myDeferredObjects.back()->GetComponent<MeshInstancedComponent>()->AddInstance(offset);
	//}
	//myDeferredObjects.back()->GetComponent<MeshInstancedComponent>()->Init();
	//
	//myDeferredObjects.emplace_back(std::make_shared<GameObject>("Cube"));
	//myDeferredObjects.back()->AddComponent(std::make_shared<MeshComponent>(*myDeferredObjects.back(), AssetManager::Get().GetAsset<MeshAsset>(L"CubeMesh")));
	//myDeferredObjects.back()->AddComponent(std::make_shared<ControllerComponent>(*myDeferredObjects.back()));
	//myDeferredObjects.back()->SetPosition(0.0f, 50.0f, -300.0f);
	//
	//myDeferredObjects.emplace_back(std::make_shared<GameObject>("Chest"));
	//myDeferredObjects.back()->SetPosition(600.f, 0.f, 0.f);
	//myDeferredObjects.back()->RotateAroundY(180.0f);
	//myDeferredObjects.back()->AddComponent(std::make_shared<MeshComponent>(*myDeferredObjects.back(), AssetManager::Get().GetAsset<MeshAsset>(L"SM_Chest")));
	//myDeferredObjects.back()->AddComponent(std::make_shared<MaterialComponent>(*myDeferredObjects.back(), AssetManager::Get().GetAsset<MaterialAsset>(L"M_ChestMaterial")));
	//myDeferredObjects.back()->AddComponent(std::make_shared<ParticleSystemComponent>(*myDeferredObjects.back()));
	//myDeferredObjects.back()->GetComponent<ParticleSystemComponent>()->Init(ParticleEmitterType::Other);
	//
	//myDeferredObjects.emplace_back(std::make_shared<GameObject>("Bro"));
	//myDeferredObjects.back()->SetPosition(0.0f, 0.f, -300.f);
	//myDeferredObjects.back()->RotateAroundY(180.0f);
	//myDeferredObjects.back()->AddComponent(std::make_shared<MeshComponent>(*myDeferredObjects.back(), AssetManager::Get().GetAsset<MeshAsset>(L"SK_C_TGA_Bro")));
	//myDeferredObjects.back()->AddComponent(std::make_shared<MaterialComponent>(*myDeferredObjects.back(), AssetManager::Get().GetAsset<MaterialAsset>(L"M_BroMaterial")));
	//myDeferredObjects.back()->AddComponent(std::make_shared<AnimationComponent>(*myDeferredObjects.back(), myDeferredObjects.back()->GetComponent<MeshComponent>()->GetMesh()->GetSkeleton()));
	//myDeferredObjects.back()->GetComponent<AnimationComponent>()->AddAnimation(AnimationState::Idle, AssetManager::Get().GetAsset<AnimationAsset>(L"A_C_TGA_Bro_Idle_Brething"));
	//myDeferredObjects.back()->GetComponent<AnimationComponent>()->AddAnimation(AnimationState::Waving, AssetManager::Get().GetAsset<AnimationAsset>(L"A_C_TGA_Bro_Idle_Wave"));
	//myDeferredObjects.back()->GetComponent<AnimationComponent>()->AddAnimation(AnimationState::Running, AssetManager::Get().GetAsset<AnimationAsset>(L"A_C_TGA_Bro_Run"));
	//myDeferredObjects.back()->GetComponent<AnimationComponent>()->AddAnimation(AnimationState::Walking, AssetManager::Get().GetAsset<AnimationAsset>(L"A_C_TGA_Bro_Walk"));
	//myDeferredObjects.back()->GetComponent<AnimationComponent>()->SetAnimationState(AnimationState::Running);
	//myDeferredObjects.back()->AddComponent(std::make_shared<ParticleSystemComponent>(*myDeferredObjects.back()));
	//myDeferredObjects.back()->GetComponent<ParticleSystemComponent>()->Init(ParticleEmitterType::StarParticle);
	//
	//myDeferredObjects.emplace_back(std::make_shared<GameObject>("Buddha"));
	//myDeferredObjects.back()->SetPosition(0.0f, 0.f, 600.0f);
	//myDeferredObjects.back()->RotateAroundY(180.0f);
	//myDeferredObjects.back()->AddComponent(std::make_shared< MeshComponent >(*myDeferredObjects.back(), AssetManager::Get().GetAsset<MeshAsset>(L"SM_Buddha")));
	//myDeferredObjects.back()->AddComponent(std::make_shared< MaterialComponent >(*myDeferredObjects.back(), AssetManager::Get().GetAsset<MaterialAsset>(L"M_BuddhaMaterial")));
	//
	//myForwardObjects.emplace_back(std::make_shared<GameObject>("PlaneV"));
	//myForwardObjects.back()->AddComponent(std::make_shared<MeshComponent>(*myForwardObjects.back(), AssetManager::Get().GetAsset<MeshAsset>(L"VerticalPlaneMesh")));
	//myForwardObjects.back()->RotateAroundY(180.0f);
	//myForwardObjects.back()->AddComponent(std::make_shared<MaterialComponent>(*myForwardObjects.back(), AssetManager::Get().GetAsset<MaterialAsset>(L"M_WoodMaterial")));
	//myForwardObjects.back()->SetPosition(0.0f, 100.0f, -800.0f);
	//
	//mySingleObject = std::make_shared<GameObject>("SingleObject");
	//mySingleObject->RotateAroundY(180.0f);
	//
	InitSortingLists();
}

void Scene::LoadScene(const std::filesystem::path& aPath, bool aIsNetworkLevel)
{
	myWorldBounds.Radius = 2000.0f;
	myWorldBounds.Origin = Vector3f({ 0.0f, 0.0f, 0.0f });

	myLightBuffer = std::make_shared<LightBuffer>();
	myDirLight = std::make_shared<GameObject>("DirLight");
	myPointLights.reserve(4);
	mySpotLights.reserve(4);
	InitSceneLights();
	if (!aIsNetworkLevel)
	{
		myLevelLoader.LoadLevelFromJSON(aPath, myCurrentLevel);
	}
	else
	{
		myCurrentLevel.Camera = std::make_shared<GameObject>("Camera");
		myCurrentLevel.Camera->AddComponent(std::make_shared<CameraComponent>(*myCamera));
		//myCurrentLevel.Camera->GetComponent<CameraComponent>()->Init(CommonUtilities::Vector3<float>(-40.0f, 1625.0f, -560.0f));
	}
	InitSortingLists();
	myCurrentScene = aPath.string();
}

void Scene::InitSortingLists()
{
	myDeferredSort.clear();
	for (auto& object : myCurrentLevel.DeferredObjects)
	{
		myDeferredSort.emplace_back(SortingInfo());
		myDeferredSort.back().CameraDistance = 0.0f;
		myDeferredSort.back().Object = object;
	}
	myForwardSort.clear();
	for (auto& object : myCurrentLevel.ForwardObjects)
	{
		myForwardSort.emplace_back(SortingInfo());
		myForwardSort.back().CameraDistance = 0.0f;
		myForwardSort.back().Object = object;
	}
}

//void Scene::AddToScene(const std::filesystem::path& aPath)
//{
//	if (!AssetManager::Get().RegisterAndLoadAsset(aPath))
//	{
//		return;
//	}
//
//	RegistryID lastID = AssetManager::Get().GetLastRegistered();
//
//	switch (lastID.Type)
//	{
//	case AssetType::Mesh:
//		AddMeshToScene(lastID.Name);
//		break;
//	case AssetType::Texture:
//		AddTextureToMesh(aPath);
//		break;
//	case AssetType::Animation:
//		AddAnimationToMesh(lastID.Name);
//		break;
//	case AssetType::Material:
//		AddMaterialToMesh(lastID.Name);
//		break;
//	default:
//		break;
//	}
//}
//
//
//void Scene::AddMeshToScene(const std::wstring& aFileName)
//{
//	if (mySingleObject->GetComponent<MeshComponent>())
//	{
//		mySingleObject->RemoveComponent<MeshComponent>();
//	}
//	mySingleObject->AddComponent(std::make_shared<MeshComponent>(*mySingleObject, AssetManager::Get().GetAsset<MeshAsset>(aFileName)));
//}
//
//void Scene::AddAnimationToMesh(const std::wstring& aFileName)
//{
//	if (mySingleObject->GetComponent<MeshComponent>().get() == nullptr)
//	{
//		LOG(SceneLog, Error, "You first need to add a skeletal mesh!");
//		return;
//	}
//	else
//	{
//		if (mySingleObject->GetComponent<MeshComponent>()->GetMesh()->GetSkeleton().bones.size() <= 0)
//		{
//			LOG(SceneLog, Error, "Can't add animation to static mesh!");
//			return;
//		}
//	}
//	if (!mySingleObject->GetComponent<AnimationComponent>())
//	{
//		mySingleObject->AddComponent(std::make_shared<AnimationComponent>(*mySingleObject, mySingleObject->GetComponent<MeshComponent>()->GetMesh()->GetSkeleton()));
//	}
//	mySingleObject->GetComponent<AnimationComponent>()->AddAndSetAnimation(AssetManager::Get().GetAsset<AnimationAsset>(aFileName));
//}
//
//void Scene::AddMaterialToMesh(const std::wstring& aFileName)
//{
//	if (mySingleObject->GetComponent<MeshComponent>().get() == nullptr)
//	{
//		LOG(SceneLog, Error, "You first need to add a mesh!");
//		return;
//	}
//	if (mySingleObject->GetComponent<MaterialComponent>())
//	{
//		mySingleObject->RemoveComponent<MaterialComponent>();
//	}
//	mySingleObject->AddComponent(std::make_shared<MaterialComponent>(*mySingleObject, AssetManager::Get().GetAsset<MaterialAsset>(aFileName)));
//}
//
//void Scene::AddTextureToMesh(const std::wstring& aFileName)
//{
//	if (mySingleObject->GetComponent<MeshComponent>().get() == nullptr)
//	{
//		LOG(SceneLog, Error, "You first need to add a mesh!");
//		return;
//	}
//	if (mySingleObject->GetComponent<TextureComponent>())
//	{
//		mySingleObject->RemoveComponent<TextureComponent>();
//	}
//	mySingleObject->AddComponent(std::make_shared<TextureComponent>(*mySingleObject, AssetManager::Get().GetAsset<TextureAsset>(aFileName)));
//}



bool Scene::AddPlayer(const CU::Vector3f& aSpawnPosition, unsigned char anID, bool aIsUser)
{
	myIDtoIndex.insert({ anID, static_cast<unsigned>(myCurrentLevel.DeferredObjects.size()) });
	myCurrentLevel.DeferredObjects.emplace_back(std::make_shared<GameObject>("Unnamed", anID));
	std::shared_ptr<GameObject> newObject = myCurrentLevel.DeferredObjects.back();
	newObject->SetPosition(aSpawnPosition);
	newObject->AddComponent(std::make_shared<MeshComponent>(*newObject, AssetManager::Get().GetAsset<MeshAsset>("SphereMesh")));
	if (aIsUser)
	{
		newObject->AddComponent(std::make_shared<ControllerComponent>(*newObject));
		newObject->AddComponent(std::make_shared<MaterialComponent>(*newObject, AssetManager::Get().GetAsset<MaterialAsset>("M_ChestMaterial")));
	}
	return true;
}

bool Scene::AddObject(const CU::Vector3f& aPosition, const char* aMesh, unsigned char anID, bool aIsActive, bool aIsUser)
{
	myIDtoIndex.insert({ anID, static_cast<unsigned>(myCurrentLevel.DeferredObjects.size()) });
	myCurrentLevel.DeferredObjects.emplace_back(std::make_shared<GameObject>("Unnamed", anID));
	std::shared_ptr<GameObject> newObject = myCurrentLevel.DeferredObjects.back();
	newObject->SetPosition(aPosition);
	newObject->SetIsActive(aIsActive);
	std::filesystem::path meshName = aMesh;
	newObject->AddComponent(std::make_shared<MeshComponent>(*newObject, AssetManager::Get().GetAsset<MeshAsset>(meshName.wstring())));
	if (aIsUser)
	{
		newObject->AddComponent(std::make_shared<MaterialComponent>(*newObject, AssetManager::Get().GetAsset<MaterialAsset>("M_BuddhaMaterial")));
	}
	return true;
}

bool Scene::RemoveObjectByID(unsigned char anID)
{
	myCurrentLevel.DeferredObjects.erase(myCurrentLevel.DeferredObjects.begin() + myIDtoIndex.at(anID));
	myIDtoIndex.erase(anID);
	return true;
}

void Scene::ClearScene()
{
	myShouldClear = true;
}

void Scene::UpdateScene(const float aDeltaTime)
{
	if (myShouldClear)
	{
		ResetScene(true);
		return;
	}
	myCurrentLevel.Camera->Update(aDeltaTime);
	for (auto& object : myCurrentLevel.ForwardObjects)
	{
		object->Update(aDeltaTime);
	}
	for (auto& object : myCurrentLevel.DeferredObjects)
	{
		object->Update(aDeltaTime);
	}
	UpdateLights();
	SortObjects();
}

void Scene::RenderScene()
{
	auto& renderer = MainSingleton::Get().GetRenderer();
	renderer.Enqueue<GCmdSetDebugBuffer>(myDebugBuffer);

	renderer.Enqueue<GCmdBeginEvent>("DirLightShadowMapping");
	renderer.Enqueue<GCmdSetVertexShader>("Default_VS");
	renderer.Enqueue<GCmdSetPixelShader>("None");
	renderer.Enqueue<GCmdChangePipelineState>(static_cast<int>(PipelineStates::DirlightShadowMapping));
	myDirLight->GetComponent<ShadowCameraComponent>()->Render();
	
	for (auto& object : myDeferredSort)
	{
		object.Object->Render();
	}
	for (auto& object : myForwardSort)
	{
		object.Object->Render();
	}
	renderer.Enqueue<GCmdEndEvent>();

	//renderer.Enqueue<GCmdBeginEvent>("PointLightShadowMapping");
	//renderer.Enqueue<GCmdChangePipelineState>(static_cast<int>(PipelineStates::PointlightShadowMapping));
	//myPointLights[0]->GetComponent<ShadowCameraComponent>()->Render();
	////TODO: draw on all shadowmaps!
	//
	//for (auto& object : myDeferredSort)
	//{
	//	object.Object->Render();
	//}
	//for (auto& object : myForwardSort)
	//{
	//	object.Object->Render();
	//}
	//renderer.Enqueue<GCmdEndEvent>();
	//
	//renderer.Enqueue<GCmdBeginEvent>("SpotLightShadowMapping");
	//renderer.Enqueue<GCmdChangePipelineState>(static_cast<int>(PipelineStates::SpotlightShadowMapping));
	//mySpotLights[0]->GetComponent<ShadowCameraComponent>()->Render();
	//for (auto& object : myDeferredSort)
	//{
	//	object.Object->Render();
	//}
	//for (auto& object : myForwardSort)
	//{
	//	object.Object->Render();
	//}
	//renderer.Enqueue<GCmdEndEvent>();
	
	renderer.Enqueue<GCmdBeginEvent>("Deferred Rendering");
	renderer.Enqueue<GCmdSetPixelShader>("GBuffer_PS");
	renderer.Enqueue<GCmdChangePipelineState>(static_cast<int>(PipelineStates::DeferredRendering));
	myCurrentLevel.Camera->Render(); 

	for (auto& object : myDeferredSort)
	{
		object.Object->Render();
	}

	renderer.Enqueue<GCmdSetLightBuffer>(myLightBuffer);
	renderer.Enqueue<GCmdClearBackBuffer>();
	renderer.Enqueue <GCmdSetMarker>("Light");
	renderer.Enqueue<GCmdSetPixelShader>("Dirlight_PS");
	renderer.Enqueue<GCmdSetVertexShader>("Quad_VS");
	renderer.Enqueue<GCmdChangePipelineState>(static_cast<int>(PipelineStates::DirlightRendering));
	renderer.Enqueue<GCmdDrawQuad>();
	renderer.Enqueue<GCmdSetPixelShader>("Pointlight_PS");
	renderer.Enqueue<GCmdChangePipelineState>(static_cast<int>(PipelineStates::PointlightRendering));
	renderer.Enqueue<GCmdDrawQuad>();
	renderer.Enqueue<GCmdSetPixelShader>("Spotlight_PS");
	renderer.Enqueue<GCmdChangePipelineState>(static_cast<int>(PipelineStates::SpotlightRendering));
	renderer.Enqueue<GCmdDrawQuad>();
	renderer.Enqueue<GCmdClearTextureResource>(119);
	renderer.Enqueue<GCmdEndEvent>();

	renderer.Enqueue<GCmdBeginEvent>("Forward Rendering");
	renderer.Enqueue<GCmdSetPixelShader>("Default_PS");
	renderer.Enqueue<GCmdSetVertexShader>("Default_VS");
	renderer.Enqueue<GCmdChangePipelineState>(static_cast<int>(PipelineStates::ForwardRendering));

	for (auto& object : myForwardSort)
	{
		object.Object->Render();
	}
	renderer.Enqueue<GCmdEndEvent>();
	renderer.Enqueue<GCmdClearTextureResource>(100);

	renderer.Enqueue<GCmdBeginEvent>("Particles");
	for (auto& object : myDeferredSort)
	{
		if (object.Object->GetComponent<ParticleSystemComponent>())
		{
			renderer.Enqueue<GCmdRenderParticles>(object.Object->GetTransform(), object.Object->GetComponent<ParticleSystemComponent>()->GetParticleSystem());
		}
	}

	renderer.Enqueue<GCmdEndEvent>();


	renderer.Enqueue<GCmdBeginEvent>("Post Processing");
	renderer.Enqueue<GCmdTonemap>();
	renderer.Enqueue<GCmdBloom>();
	if (myDebugBuffer->SSAOActive)
	{
		renderer.Enqueue<GCmdSSAO>();
	}
	renderer.Enqueue<GCmdEndEvent>();

	renderer.SwitchUpdateIntermediate();

}

void Scene::PresentScene()
{
	GraphicsEngine::Get().EndFrame();
}

void Scene::CloseScene()
{
	CU::InputMapper::GetInputMapper().UnRegister(ActionEventID::ChangeTonemap, this);
	CU::InputMapper::GetInputMapper().UnRegister(ActionEventID::ChangeLuminanceMode, this);
	CU::InputMapper::GetInputMapper().UnRegister(ActionEventID::ExposureUp, this);
	CU::InputMapper::GetInputMapper().UnRegister(ActionEventID::ExposureDown, this);
}

void Scene::RecieveEvent(const ActionEvent& anEvent)
{
	switch (anEvent.Id)
	{
	case ActionEventID::ChangeTonemap:
		myDebugBuffer->Tonemap = (myDebugBuffer->Tonemap + 1) % 5;
		return;
	case ActionEventID::ExposureUp:
		myDebugBuffer->Exposure += 0.1f;
		return;
	case ActionEventID::ExposureDown:
		myDebugBuffer->Exposure -= 0.1f;
		return;
	case ActionEventID::ChangeLuminanceMode:
		myDebugBuffer->LuminanceMode = (myDebugBuffer->LuminanceMode + 1) % 4;
		return;
	case ActionEventID::Toggle_SSAO:
		myDebugBuffer->SSAOActive = (myDebugBuffer->SSAOActive + 1) % 2;
		return;
	default:
		break;
	}
}

Level& Scene::GetCurrentLevel()
{
	return myCurrentLevel;
}

std::shared_ptr<GameObject>& Scene::GetObjectByID(unsigned char anID)
{
	assert(myIDtoIndex.contains(anID) && "Trying to get object by ID but object is not in the scene!");
	return myCurrentLevel.DeferredObjects[myIDtoIndex.at(anID)];
}

std::vector<SortingInfo> Scene::SortObjectsBackToFront(std::vector<std::shared_ptr<GameObject>> someObjects)
{
	std::vector<SortingInfo> backToFrontRenderCue;
	for (std::shared_ptr<GameObject> object : someObjects)
	{
		SortingInfo info;
		info.CameraDistance = (object->GetPosition() - myCamera->GetPosition()).LengthSqr();
		info.Object = object;
		backToFrontRenderCue.push_back(info);
	}

	std::sort(backToFrontRenderCue.begin(), backToFrontRenderCue.end(), [](SortingInfo a, SortingInfo b) { return (a.CameraDistance) < b.CameraDistance; });
	return backToFrontRenderCue;
}

std::vector<SortingInfo> Scene::SortObjectsFrontToBack(std::vector<std::shared_ptr<GameObject>> someObjects) //TODO: don't copy whole vectors...
{
	std::vector<SortingInfo> frontToBackRenderCue;
	for (std::shared_ptr<GameObject> object : someObjects)
	{
		SortingInfo info;
		info.CameraDistance = (object->GetPosition() - myCamera->GetPosition()).LengthSqr();
		info.Object = object;
		frontToBackRenderCue.emplace_back(info);
	}


	std::sort(frontToBackRenderCue.begin(), frontToBackRenderCue.end(), [](SortingInfo a, SortingInfo b) { return a.CameraDistance > b.CameraDistance; });
	return frontToBackRenderCue;
}

void Scene::SortObjects()
{
	for (auto& deferredObject : myDeferredSort)
	{
		deferredObject.CameraDistance = (deferredObject.Object->GetPosition() - myCurrentLevel.Camera->GetPosition()).LengthSqr();
	}
	std::sort(myDeferredSort.begin(), myDeferredSort.end(), [](SortingInfo a, SortingInfo b) { return a.CameraDistance > b.CameraDistance; });

	for (auto& forwardObject : myForwardSort)
	{
		forwardObject.CameraDistance = (forwardObject.Object->GetPosition() - myCurrentLevel.Camera->GetPosition()).LengthSqr();
	}
	std::sort(myForwardSort.begin(), myForwardSort.end(), [](SortingInfo a, SortingInfo b) { return (a.CameraDistance) < b.CameraDistance; });
}

void Scene::InitSceneLights()
{
	myDirLight->AddComponent(std::make_shared<DirLightComponent>(*myDirLight));
	CU::Vector3f lightColor = CU::Vector3<float>( 0.95f, 0.95f, 0.8f );
	CU::Vector4f lightDir = CU::Vector4<float>(0.0f, -1.0f, 0.2f, 0.0f );
	myDirLight->GetComponent<DirLightComponent>()->InitDirLight(myWorldBounds, lightColor, 5.0f, lightDir);
	myDirLight->AddComponent(std::make_shared<ShadowCameraComponent>(*myDirLight));
	CU::Vector3f shadowPos = { myDirLight->GetComponent<DirLightComponent>()->GetLightData()->LightPos.x, 
		myDirLight->GetComponent<DirLightComponent>()->GetLightData()->LightPos.y, 
		myDirLight->GetComponent<DirLightComponent>()->GetLightData()->LightPos.z };
	myDirLight->GetComponent<ShadowCameraComponent>()->Init(
		myDirLight->GetComponent<DirLightComponent>()->GetLightData()->LightViewInv,
		myDirLight->GetComponent<DirLightComponent>()->GetLightData()->LightProj,
		shadowPos
		);
	
	const CU::Vector3<float> globalUpDir = CU::Vector3<float>( 0.0f, 1.0f, 0.0f );
	const float fov = static_cast<float> (90.0f * (3.14f / 180.0f));
	//const float farZ = 10000.0f;
	const float nearZ = 0.001f;
	const float resolutionX = static_cast<float>(GraphicsEngine::Get().GetRenderSize().x);
	const float resolutionY = static_cast<float>(GraphicsEngine::Get().GetRenderSize().y);
	const CU::Vector3<float> pointTargetPos = CU::Vector3<float>( -600.0f, 0.0f, 0.0f );
	CU::Vector3f pointPos = CU::Vector3<float>( -300.0f, 100.0f, 0.0f );
	CU::Vector3f pointColor = CU::Vector3<float>( 0.7f, 0.7f, 1.0f );
	CU::Matrix4x4f pointViewInv = CU::Matrix4x4<float>::LookAt(pointPos, pointTargetPos, globalUpDir);
	pointViewInv = CU::Matrix4x4<float>::GetFastInverse(pointViewInv);
	float pointRange = 500.0f;
	float pointFarZ = pointRange + 1.0f;
	CU::Matrix4x4f pointProj = CU::Matrix4x4<float>::CreatePerspectiveProjection(fov, pointFarZ, nearZ, resolutionX, resolutionY);

	myPointLights.emplace_back(std::make_shared<GameObject>("PointLight1"));
	myPointLights[0]->AddComponent(std::make_shared<PointLightComponent>(*myPointLights[0]));
	myPointLights[0]->GetComponent<PointLightComponent>()->InitPointLight(0, pointColor, 4.5f, pointRange,
		pointPos, pointProj, pointViewInv);
	myPointLights[0]->AddComponent(std::make_shared<ShadowCameraComponent>(*myPointLights[0]));
	myPointLights[0]->GetComponent<ShadowCameraComponent>()->Init(pointViewInv, pointProj, pointPos);
	myLightBuffer->PointLightCount++;
	
	
	CU::Vector3f spotPos = CU::Vector3<float>( 600.0f, 400.0f, 0.0f );
	CU::Vector3f spotColor = CU::Vector3<float>( 1.f, 1.f, 1.f );
	CU::Vector3f spotDir = CU::Vector3<float>( 0.0f, -1.0f, 0.f );
	float spotInnerConeAngle = 3.14f * 0.125f;
	float spotOuterConeAngle = 3.14f * 0.25f;
	float spotRange = 500.0f;
	float spotFarZ = spotRange + 10.0f;
	const CU::Vector3<float> spotTargetPos = CommonUtilities::Vector3<float>( 600.0f, 0.0f, 0.0f );
	CU::Matrix4x4f spotViewInv = CU::Matrix4x4<float>::LookAt(spotPos, spotTargetPos, globalUpDir);
	spotViewInv = CU::Matrix4x4<float>::GetFastInverse(spotViewInv);
	CU::Matrix4x4f spotProj = CU::Matrix4x4<float>::CreatePerspectiveProjection(fov, spotFarZ, nearZ, resolutionX, resolutionY);
	mySpotLights.emplace_back(std::make_shared<GameObject>("PointLight1"));
	mySpotLights[0]->AddComponent(std::make_shared<SpotLightComponent>(*mySpotLights[0]));
	mySpotLights[0]->GetComponent<SpotLightComponent>()->InitSpotLight(0, spotColor, 0.5f, spotRange, spotDir,
		spotPos, spotInnerConeAngle, spotOuterConeAngle, spotProj, spotViewInv);
	mySpotLights[0]->AddComponent(std::make_shared<ShadowCameraComponent>(*mySpotLights[0]));
	mySpotLights[0]->GetComponent<ShadowCameraComponent>()->Init(spotViewInv, spotProj, spotPos);
	myLightBuffer->SpotLightCount++;
}

void Scene::UpdateLights()
{
	myLightBuffer->DirLight = *myDirLight->GetComponent<DirLightComponent>()->GetLightData();
	for (size_t index = 0; index < myLightBuffer->PointLightCount; index++)
	{
		myLightBuffer->PointLights[index] = *myPointLights[index]->GetComponent<PointLightComponent>()->GetLightData();
	}
	for (size_t index = 0; index < myLightBuffer->SpotLightCount; index++)
	{
		myLightBuffer->SpotLights[index] = *mySpotLights[index]->GetComponent<SpotLightComponent>()->GetLightData();
	}
}

void Scene::UpdateDebugInfo()
{
}

void Scene::ResetScene(bool aShouldReload)
{
	myCurrentLevel.DeferredObjects.clear();
	myCurrentLevel.ForwardObjects.clear();
	myCurrentLevel.Camera = nullptr;
	myDeferredSort.clear();
	myForwardSort.clear();
	myShouldClear = false;

	if (aShouldReload)
	{
		LoadScene(myCurrentScene);
	}
}




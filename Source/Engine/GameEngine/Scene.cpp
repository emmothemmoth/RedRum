#include "Scene.h"
#include "GameEngine.pch.h"
#include "Scene.h"
#include "Level.h"
#include "MainSingleton.h"
#include "Components\ComponentsInclude.h"

#include "CommonUtilities\InputMapper.h"
#include "CommonUtilities/Ray.hpp"
#include "CommonUtilities/Plane.hpp"
#include "CommonUtilities/Intersection.hpp"

#include "..\AssetManager\AssetManager.h"

#include "..\GraphicsEngine\GraphicsEngine.h"
#include "..\GraphicsEngine\GraphicsCommands.h"
#include "..\GraphicsEngine\PipelineStates.h"
#include "../GraphicsEngine/Buffers/DebugBuffer.h"
#include "../GraphicsEngine/Objects/MaterialAsset.h"

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
		myLevelLoader.LoadLevelFromJSON(aPath, myCurrentLevel, myIDCounter);
	}
	else
	{
		myCurrentLevel.Camera = std::make_shared<GameObject>("Camera");
		myCurrentLevel.Camera->AddComponent(std::make_shared<CameraComponent>(*myCurrentLevel.Camera));
	}
	std::shared_ptr<CameraComponent> cameraComp = myCurrentLevel.Camera->GetLastAddedComponent<CameraComponent>();
	cameraComp->SetResolution(GraphicsEngine::Get().GetRenderSize());
	myActiveCamera = myCurrentLevel.Camera;
	for (int index = 0; index < myCurrentLevel.GameObjects.size(); ++index)
	{
		myIDtoIndex.insert({ myCurrentLevel.GameObjects.at(index)->GetID(), static_cast<unsigned>(index) });
	}

	InitSortingLists();
	myCurrentScene = aPath.string();
}

void Scene::AddObject(std::shared_ptr<GameObject> anObject)
{
	assert(anObject.get());
	anObject->SetID(myIDCounter);
	myIDtoIndex.insert({ myIDCounter++, static_cast<uint32_t>(myCurrentLevel.GameObjects.size()) });
	myCurrentLevel.GameObjects.push_back(anObject);
	InitSortingLists();
}

void Scene::RemoveObject(const uint32_t anID)
{
	assert(myIDtoIndex.contains(anID));
	myIDtoIndex.clear();
	std::vector<std::shared_ptr<GameObject>> objectList(myCurrentLevel.GameObjects.size() - 1);
	for (auto& gameObject : myCurrentLevel.GameObjects)
	{
		if (gameObject->GetID() != anID)
		{
			myIDtoIndex.insert({ gameObject->GetID(), static_cast<uint32_t>(objectList.size()) });
			objectList.emplace_back(gameObject);
		}
	}
	myCurrentLevel.GameObjects.clear();
	myCurrentLevel.GameObjects = objectList;
	InitSortingLists();
}

void Scene::InitSortingLists()
{
	mySortingList.clear();
	for (auto& object : myCurrentLevel.GameObjects)
	{
		mySortingList.emplace_back(SortingInfo());
		mySortingList.back().CameraDistance = 0.0f;
		mySortingList.back().Object = object;
	}
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
	myActiveCamera->Update(aDeltaTime);
	for (auto& object : myCurrentLevel.GameObjects)
	{
		object->Update(aDeltaTime);
	}
	UpdateLights();
	SortObjects();
}

void Scene::RenderScene()
{
	auto& renderer = MainSingleton::Get().GetRenderer();
	renderer.ChangeRenderPass(RenderStage::ShadowMapping);
	renderer.Enqueue<GCmdSetDebugBuffer>(myDebugBuffer);
	myDirLight->GetComponent<ShadowCameraComponent>()->Render();
	myActiveCamera->Render();
	
	for (auto& object : mySortingList)
	{
		object.Object->Render();
	}
	renderer.Enqueue<GCmdSetLightBuffer>(RenderStage::Deferred, myLightBuffer);
	//renderer.Enqueue<GCmdEndEvent>();
	
	//renderer.Enqueue<GCmdBeginEvent>("Deferred Rendering");
	//renderer.Enqueue<GCmdSetPixelShader>("GBuffer_PS");
	//renderer.Enqueue<GCmdChangePipelineState>(static_cast<int>(PipelineStates::DeferredRendering));

	//for (auto& object : mySortingList)
	//{
	//	object.Object->Render();
	//}

	renderer.Enqueue<GCmdClearBackBuffer>(RenderStage::Deferred);
	renderer.Enqueue<GCmdSetMarker>(RenderStage::Deferred, "Light");
	renderer.Enqueue<GCmdSetPixelShader>(RenderStage::Deferred, "Dirlight_PS");
	renderer.Enqueue<GCmdSetVertexShader>(RenderStage::Deferred, "Quad_VS");
	renderer.Enqueue<GCmdChangePipelineState>(RenderStage::Deferred, static_cast<int>(PipelineStates::DirlightRendering));
	renderer.Enqueue<GCmdDrawQuad>(RenderStage::Deferred);
	renderer.Enqueue<GCmdSetPixelShader>(RenderStage::Deferred, "Pointlight_PS");
	renderer.Enqueue<GCmdChangePipelineState>(RenderStage::Deferred, static_cast<int>(PipelineStates::PointlightRendering));
	renderer.Enqueue<GCmdDrawQuad>(RenderStage::Deferred);
	renderer.Enqueue<GCmdSetPixelShader>(RenderStage::Deferred, "Spotlight_PS");
	renderer.Enqueue<GCmdChangePipelineState>(RenderStage::Deferred, static_cast<int>(PipelineStates::SpotlightRendering));
	renderer.Enqueue<GCmdDrawQuad>(RenderStage::Deferred);
	renderer.Enqueue<GCmdClearTextureResource>(RenderStage::Deferred, 119);
	//renderer.Enqueue<GCmdEndEvent>();

	//renderer.Enqueue<GCmdBeginEvent>("Forward Rendering");
	//renderer.Enqueue<GCmdSetPixelShader>("Default_PS");
	//renderer.Enqueue<GCmdSetVertexShader>("Default_VS");
	//renderer.Enqueue<GCmdChangePipelineState>(static_cast<int>(PipelineStates::ForwardRendering));

	//for (auto& object : mySortingList)
	//{
	//	object.Object->Render();
	//}
	//renderer.Enqueue<GCmdEndEvent>();
	//renderer.Enqueue<GCmdClearTextureResource>(100);

	//renderer.Enqueue<GCmdBeginEvent>("Particles");
	//for (auto& object : mySortingList)
	//{
	//	if (object.Object->GetComponent<ParticleSystemComponent>())
	//	{
	//		renderer.Enqueue<GCmdRenderParticles>(object.Object->GetTransform(), object.Object->GetComponent<ParticleSystemComponent>()->GetParticleSystem());
	//	}
	//}

	//renderer.Enqueue<GCmdEndEvent>();


	//renderer.Enqueue<GCmdBeginEvent>("Post Processing");
	renderer.Enqueue<GCmdTonemap>(RenderStage::PostProcess);
	renderer.Enqueue<GCmdBloom>(RenderStage::PostProcess);
	if (myDebugBuffer->SSAOActive)
	{
		renderer.Enqueue<GCmdSSAO>(RenderStage::PostProcess);
	}
	//renderer.Enqueue<GCmdEndEvent>();

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

std::shared_ptr<GameObject>& Scene::GetObjectByID(uint32_t anID)
{
	assert(myIDtoIndex.contains(anID) && "Trying to get object by ID but object is not in the scene!");
	return myCurrentLevel.GameObjects[myIDtoIndex.at(anID)];
}

void Scene::SortObjects()
{
	for (auto& deferredObject : mySortingList)
	{
		deferredObject.CameraDistance = (deferredObject.Object->GetPosition() - myCurrentLevel.Camera->GetPosition()).LengthSqr();
	}
	std::sort(mySortingList.begin(), mySortingList.end(), [](SortingInfo a, SortingInfo b) { return a.CameraDistance < b.CameraDistance; });
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
	myCurrentLevel.GameObjects.clear();
	myCurrentLevel.Camera = nullptr;
	mySortingList.clear();
	myShouldClear = false;

	if (aShouldReload)
	{
		LoadScene(myCurrentScene);
	}
}




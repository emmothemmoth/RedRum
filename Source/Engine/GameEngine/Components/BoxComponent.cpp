#include "GameEngine.pch.h"
#include "BoxComponent.h"
#include "GameObject.h"
#include "MainSingleton.h"

#include "../../GraphicsEngine/Commands/GCmdDrawDebugLines.h"


BoxComponent::BoxComponent(GameObject& aParent)
	: Component(aParent)
{
	myComponentType = ComponentType::Box;
	myRenderStages.at(RenderStage::WorldSpaceUI) = true;
	myIsVisible = false;
}

BoxComponent::~BoxComponent()
{
}

void BoxComponent::Initialize(const CU::Vector3f aMin, const CU::Vector3f aMax, bool aIsAudioObstacle)
{
	myDebugObject = std::make_shared<DebugLineObject>();
	CU::Vector3f topLeftClose(aMin.x, aMax.y, aMin.z);
	CU::Vector3f bottomRightClose(aMax.x, aMin.y, aMin.z);
	CU::Vector3f topRightClose(aMax.x, aMax.y, aMin.z);

	CU::Vector3f bottomLeftFar(aMin.x, aMin.y, aMax.z);
	CU::Vector3f topLeftFar(aMin.x, aMax.y, aMax.z);
	CU::Vector3f bottomRightFar(aMax.x, aMin.y, aMax.z);

	myDebugObject->AddLine(aMin, topLeftClose);
	myDebugObject->AddLine(topLeftClose, topRightClose);
	myDebugObject->AddLine(topRightClose, bottomRightClose);
	myDebugObject->AddLine(bottomRightClose, aMin);

	myDebugObject->AddLine(bottomLeftFar, topLeftFar);
	myDebugObject->AddLine(topLeftFar, aMax);
	myDebugObject->AddLine(aMax, bottomRightFar);
	myDebugObject->AddLine(bottomRightFar, bottomLeftFar);

	myDebugObject->AddLine(aMin, bottomLeftFar);
	myDebugObject->AddLine(topLeftClose, topLeftFar);

	myDebugObject->AddLine(topRightClose, aMax);
	myDebugObject->AddLine(bottomRightClose, bottomRightFar);
	myDebugObject->SetColor(DebugColor::Yellow);
	myDebugObject->Initialize();

	myParent.OnComponentSelected.AddRaw(this, &BoxComponent::ShowLines);
	myParent.OnComponentDeselected.AddRaw(this, &BoxComponent::HideLines);

	if (!aIsAudioObstacle) return;

	AABBCollider boxShape;
	boxShape.MinPoint = aMin;
	boxShape.MaxPoint = aMax;

	myCollider.Shape = boxShape;

	auto handle = MainSingleton::Get().GetAudioEngine().RegisterAudioObstacle(
		myAbsorberSettings,
		myCollider, myParent.GetTransform());
	if (handle)
	{
		myObstacleHandle = handle.value();
	}
}

void BoxComponent::Update(const float aDeltaTime)
{
	if (myObstacleHandle == UINT32_MAX) return;
	aDeltaTime;
	if (myParent.IsDirty())
	{
		MainSingleton::Get().GetAudioEngine().UpdateAudioObstacle(
			myObstacleHandle,
			myAbsorberSettings,
			myCollider,
			myParent.GetTransform()
		);
	}
}


void BoxComponent::Render()
{
	if (!myIsVisible) return;
	auto& renderer = RENDERER;
	renderer.Enqueue<GCmdDrawDebugLines>(RenderStage::WorldSpaceUI, myDebugObject, myParent.GetTransform());
}

void BoxComponent::ShowLines()
{
	myIsVisible = true;
}

void BoxComponent::HideLines()
{
	myIsVisible = false;
}

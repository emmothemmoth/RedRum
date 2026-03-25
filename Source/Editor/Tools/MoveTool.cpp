#include "MoveTool.h"

#include "CommonUtilities/Intersection.hpp"

#include "../../Engine/GraphicsEngine/GraphicsEngine.h"
#include <iostream>

void MoveTool::Update(const InputState& anInputState, const float aDeltaTime)
{
	currentDeltaTime = aDeltaTime;
	if (anInputState.MouseHeld)
	{
		PerformAction(anInputState);
	}
	else if(anInputState.MouseReleased)
	{
		myDragState.Active = false;
	}
}

void MoveTool::PerformAction(const InputState& anInputState)
{
	assert(mySelectedPartID != 0);
	assert(mySelectedObjects.size() > 0);
	std::shared_ptr<GameObject> object = myScene->GetObjectByID(mySelectedObjects.back());

	Gizmo_Axis transformAxis = static_cast<Gizmo_Axis>(mySelectedPartID);
	CU::Vector3f axis;
	switch (transformAxis)
	{
	case Gizmo_Axis::Gizmo_X: axis = { object->GetTransform().GetRow(1).x, object->GetTransform().GetRow(1).y, object->GetTransform().GetRow(1).z }; break;
	case Gizmo_Axis::Gizmo_Y: axis = { object->GetTransform().GetRow(2).x, object->GetTransform().GetRow(2).y, object->GetTransform().GetRow(2).z }; break;
	case Gizmo_Axis::Gizmo_Z: axis = { object->GetTransform().GetRow(3).x, object->GetTransform().GetRow(3).y, object->GetTransform().GetRow(3).z }; break;
	default: return;
	}
	axis.Normalize();

	// --- 1. BUILD THE RAY ---
	std::shared_ptr<GameObject> camera = myScene->GetActiveCamera();
	CU::Vector2f viewportSize = GraphicsEngine::Get().GetViewPortSize();
	float ndcX = (2.0f * anInputState.MousePos.x / viewportSize.x) - 1.0f;
	float ndcY = 1.0f - (2.0f * anInputState.MousePos.y / viewportSize.y);

	CU::Vector4f nearPoint(ndcX, ndcY, 0.0f, 1.0f);
	CU::Vector4f farPoint(ndcX, ndcY, 1.0f, 1.0f);

	CU::Matrix4x4f invViewProj = (camera->GetComponent<CameraComponent>()->GetViewInverse() * camera->GetComponent<CameraComponent>()->GetClipMatrix());
	invViewProj = invViewProj.GetInverse();

	CU::Vector4f nearWorld = nearPoint * invViewProj;
	CU::Vector4f farWorld = farPoint * invViewProj;
	nearWorld /= nearWorld.w;
	farWorld /= farWorld.w;

	CU::Ray<float> ray;
	CU::Vector3f rayDir = { farWorld.x - nearWorld.x, farWorld.y - nearWorld.y, farWorld.z - nearWorld.z };
	rayDir.Normalize();
	CU::Vector3f nearWorld3 = { nearWorld.x, nearWorld.y, nearWorld.z };
	ray.InitWithOriginAndDirection(nearWorld3, rayDir);

	// --- 2. INITIALIZE THE DRAG (RUNS ONCE PER CLICK) ---
	if (!myDragState.Active)
	{
		myDragState.Active = true;
		myDragState.InitialObjectPosition = object->GetPosition();

		CU::Vector4f camfwd = camera->GetTransform().GetRow(3);
		CU::Vector3f cameraForward = { camfwd.x, camfwd.y, camfwd.z };

		CU::Vector3f t = axis.Cross(cameraForward);
		if (t.LengthSqr() < 0.001f)
		{
			CU::Vector3f camRight = { camera->GetTransform().GetRow(1).x,camera->GetTransform().GetRow(1).y, camera->GetTransform().GetRow(1).z };
			t = axis.Cross(camRight);
		}

		CU::Vector3f planeNormal = t.Cross(axis);
		planeNormal.Normalize();

		// Build the plane ONCE using the Initial Position
		myDragState.DragPlane.InitWithPointAndNormal(myDragState.InitialObjectPosition, planeNormal);

		// Record exactly where the mouse hit the plane when we started
		CU::Vector3f intersection;
		CU::IntersectionPlaneRay(myDragState.DragPlane, ray, intersection);
		myDragState.InitialIntersection = intersection;

		return; // Wait for the next frame to actually move it!
	}

	// --- 3. EXECUTE THE DRAG (RUNS EVERY FRAME WHILE HELD) ---

	CU::Vector3f currentIntersection;
	// Raycast against the STATIC plane we saved in Step 2
	if (CU::IntersectionPlaneRay(myDragState.DragPlane, ray, currentIntersection))
	{
		// Compare where the mouse is NOW vs where it started
		CU::Vector3f totalDelta = currentIntersection - myDragState.InitialIntersection;

		// Project that delta onto the axis
		float movement = totalDelta.Dot(axis);

		// Set the position relative to the INITIAL position
		object->SetPosition(myDragState.InitialObjectPosition + (axis * movement));
	}
}

bool MoveTool::Done()
{
	return !myDragState.Active;
}

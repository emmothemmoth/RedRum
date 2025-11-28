#pragma once
#include "Plane.hpp"
#include "AABB3D.hpp"
#include "Sphere.hpp"
#include "Ray.hpp"
#include "Vector"
#include <vector>
namespace CommonUtilities
{
	// If the ray is parallel to the plane, aOutIntersectionPoint remains unchanged. If
// the ray is in the plane, true is returned, if not, false is returned. If the ray
// isn't parallel to the plane, the intersection point is stored in
// aOutIntersectionPoint and true returned.
	template <typename T>
	bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
	{
		Vector3<T> e = aPlane.GetPoint0() - aRay.GetOrigin();
		//If parallell
		if (aRay.GetDirection().Dot(aPlane.GetNormal()) == 0)
		{
			return false;
		}
		else
		{
			T product = (aPlane.GetPoint0().Dot(aPlane.GetNormal()) - (aRay.GetOrigin().Dot(aPlane.GetNormal())))
				/ (aRay.GetDirection().Dot(aPlane.GetNormal()));
			aOutIntersectionPoint = product * aRay.GetDirection();
			return true;
		}
	}
	// If the ray intersects the AABB, true is returned, if not, false is returned.
	// A ray in one of the AABB's sides is counted as intersecting it.
	template <typename T>
	bool IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay)
	{
		//Origin inside box?
		if (aRay.GetOrigin().x < aAABB.GetMax().x && aRay.GetOrigin().x > aAABB.GetMax().x)
		{
			if (aRay.GetOrigin().y < aAABB.GetMax().y && aRay.GetOrigin().y > aAABB.GetMax().y)
			{
				if (aRay.GetOrigin().z < aAABB.GetMax().z && aRay.GetOrigin().z > aAABB.GetMax().z)
				{
					return true;
				}
			}
		}

		if (aRay.GetOrigin().x < aAABB.GetMin().x && aRay.GetDirection().x <= 0)
		{
			return false;
		}
		else if (aRay.GetOrigin().x > aAABB.GetMax().x && aRay.GetDirection().x >= 0)
		{
			return false;
		}

		if (aRay.GetOrigin().y < aAABB.GetMin().y && aRay.GetDirection().y <= 0)
		{
			return false;
		}
		else if (aRay.GetOrigin().y > aAABB.GetMax().y && aRay.GetDirection().y >= 0)
		{
			return false;
		}

		if (aRay.GetOrigin().z < aAABB.GetMin().z && aRay.GetDirection().z <= 0)
		{
			return false;
		}
		else if (aRay.GetOrigin().z > aAABB.GetMax().z && aRay.GetDirection().z >= 0)
		{
			return false;
		}

		std::vector<Plane<T>> AABBPlanes;
		Vector3<T> frontNormal{ 0, 0, -1 };
		Plane<T> frontSide;
		frontSide.InitWithPointAndNormal(aAABB.GetMin(), frontNormal);
		AABBPlanes.push_back(frontSide);

		Vector3<T> rightNormal{ 1, 0, 0 };
		Plane<T> rightSide;
		rightSide.InitWithPointAndNormal(aAABB.GetMax(), rightNormal);
		AABBPlanes.push_back(rightSide);

		Vector3<T> backNormal{ 0, 0, 1 };
		Plane<T> backSide;
		backSide.InitWithPointAndNormal(aAABB.GetMax(), backNormal);
		AABBPlanes.push_back(backSide);

		Vector3<T> leftNormal{ -1, 0, 0 };
		Plane<T> leftSide;
		leftSide.InitWithPointAndNormal(aAABB.GetMin(), leftNormal);
		AABBPlanes.push_back(leftSide);

		Vector3<T> bottomNormal{ 0, -1, 0 };
		Plane<T> bottomSide;
		bottomSide.InitWithPointAndNormal(aAABB.GetMin(), bottomNormal);
		AABBPlanes.push_back(bottomSide);

		Vector3<T> topNormal{ 0, 1, 0 };
		Plane<T> topSide;
		topSide.InitWithPointAndNormal(aAABB.GetMax(), topNormal);
		AABBPlanes.push_back(topSide);

		Vector3<T> intersection;
		for (int index = 0; index < AABBPlanes.size(); ++index)
		{
			const Plane<T> aPlane = static_cast<const Plane<T>> (AABBPlanes[index]);
			if (IntersectionPlaneRay(aPlane, aRay, intersection))
			{
				const Vector3<T> position = intersection;
				if (aAABB.IsInside(position))
				{
					return true;
				}
			}
		}
		return false;

	}

	// If the ray intersects the sphere, true is returned, if not, false is returned.
	// A ray intersecting the surface of the sphere is considered as intersecting it.
	template <typename T>
	bool IntersectionSphereRay(const Sphere<T>& aSphere, const Ray<T>& aRay)
	{
		Vector3<T> e = aSphere.GetPosition() - aRay.GetOrigin();
		T a = e.Dot(aRay.GetDirection().GetNormalized());
		T b2 = (e.LengthSqr() - (a * a));
		T b = sqrt(b2);
		if (b <= aSphere.GetRadius())
		{
			return true;
		}
		return false;
	}

}

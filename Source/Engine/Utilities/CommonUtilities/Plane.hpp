#pragma once
#include "Vector3.hpp"

namespace CommonUtilities
{
	template <class T>
		class Plane
	{
	public:
		// Default constructor.
		Plane()
		{
			myPoint0.x = 0;
			myPoint0.y = 0;
			myPoint0.z = 0;
			myPoint1.x = 0;
			myPoint1.y = 0;
			myPoint1.z = 0;
			myPoint2.x = 0;
			myPoint2.y = 0;
			myPoint2.z = 0;

		}

		// Constructor taking three points where the normal is (aPoint1 - aPoint0) x (aPoint2 -aPoint0).
		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
		{
			myPoint0 = aPoint0;
			myPoint1 = aPoint1;
			myPoint2 = aPoint2;
			Vector3<T> normalArgument1 = aPoint1 - aPoint0;
			Vector3<T> normalArgument2 = aPoint2 - aPoint1;
			assert(normalArgument1.Dot(normalArgument2) != 0);
			myNormal = normalArgument1.Cross(normalArgument2);
			myNormal.Normalize();
		}

		// Constructor taking a point and a normal.
		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal)
		{
			myNormal = aNormal;
			myPoint0 = aPoint0;
		}

		// Init the plane with three points, the same as the constructor above.
		void InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
		{
			Vector3<T> vector1 = aPoint1 - aPoint0;
			Vector3<T> vector2 = aPoint2 - aPoint0;
			Vector3<T> vector1Normal = vector1.GetNormalized();

			assert(!((vector2.Dot(vector1Normal) == 0)) && "Points not able to define the plane");

			myPoint0 = aPoint0;
			myPoint1 = aPoint1;
			myPoint2 = aPoint2;
			myNormal = vector1.Cross(vector2);
			myNormal.Normalize();
		}

		// Init the plane with a point and a normal, the same as the constructor above.
		void InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal)
		{
			myPoint0 = aPoint;
			myNormal = aNormal;
		}

		// Returns whether a point is inside the plane: it is inside when the point is on the
		//plane or on the side the normal is pointing away from.
		bool IsInside(const Vector3<T>& aPosition) const
		{
			Vector3<T> diffVector = aPosition - myPoint0;
			if (diffVector.Dot(myNormal) <= 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		// Returns the normal of the plane.
		const Vector3<T>& GetNormal() const
		{
			return myNormal;
		}

		Vector3<T> GetPoint0() const
		{
			return myPoint0;
		}
		Vector3<T> GetPoint1() const
		{
			return myPoint0;
		}
		Vector3<T> GetPoint2() const
		{
			return myPoint0;
		}
	private:
		Vector3<T> myPoint0;
		Vector3<T> myPoint1;
		Vector3<T> myPoint2;
		Vector3<T> myNormal;

	};
}

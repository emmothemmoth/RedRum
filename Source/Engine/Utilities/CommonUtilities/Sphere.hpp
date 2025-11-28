#pragma once

#pragma once
#include "Vector.hpp"

namespace CommonUtilities
{
	template <class T>
		class Sphere
	{
	public:
		// Default constructor: there is no sphere, the radius is zero and the position is the zero vector.
		Sphere()
		{
			myPosition.x = 0;
			myPosition.y = 0;
			myPosition.z = 0;
			myRadius = 0;
		}

		// Copy constructor.
		Sphere(const Sphere<T>& aSphere)
		{
			myPosition = aSphere.myPosition;
			myRadius = aSphere.myRadius;
		}

		// Constructor that takes the center position and radius of the sphere.
		Sphere(const Vector3<T>& aCenter, T aRadius)
		{
			myPosition = aCenter;
			myRadius = aRadius;
		}

		// Init the sphere with a center and a radius, the same as the constructor above.
		void InitWithCenterAndRadius(const Vector3<T>& aCenter, T aRadius)
		{
			myPosition = aCenter;
			myRadius = aRadius;
		}

		// Returns whether a point is inside the sphere: it is inside when the point is on the
		// sphere surface or inside of the sphere.
		bool IsInside(const Vector3<T>& aPosition) const
		{
			Vector3<T> distance = aPosition - myPosition;
			T magnitude = distance.Length();
			if (magnitude <= myRadius)
			{
				if (magnitude <= myRadius)
				{
					if (magnitude <= myRadius)
					{
						return true;
					}
				}
			}
			return false;
		}
		Vector3<T> GetPosition() const
		{
			return myPosition;
		}
		T GetRadius() const
		{
			return myRadius;
		}
		void SetPosition(const Vector3<T> aPosition)
		{
			myPosition = aPosition;
		}
		void SetRadius(const float aRadius)
		{
			assert(aRadius >= 0);
			myRadius = aRadius;
		}
	private:
		Vector3<T> myPosition;
		T myRadius;

	};
}

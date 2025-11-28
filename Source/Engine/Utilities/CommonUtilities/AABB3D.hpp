#pragma once
#include "Vector3.hpp"

namespace CommonUtilities
{
	template <class T>
	class AABB3D
	{
	public:
		// Default constructor: there is no AABB, both min and max points are the zero vector.
		AABB3D()
		{
			myMin.x = 0;
			myMin.y = 0;
			myMin.z = 0;

			myMax.x = 0;
			myMax.y = 0;
			myMax.z = 0;
		}

		// Copy constructor.
		AABB3D(const AABB3D<T>& aAABB3D)
		{
			myMin = aAABB3D.myMin;
			myMax = aAABB3D.myMax;
		}

		// Constructor taking the positions of the minimum and maximum corners.
		AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax)
		{
			myMin = aMin;
			myMax = aMax;
		}

		// Init the AABB with the positions of the minimum and maximum corners, same as
		// the constructor above.
		void InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax)
		{
			myMin = aMin;
			myMax = aMax;
		}

		// Returns whether a point is inside the AABB: it is inside when the point is on any
		// of the AABB's sides or inside of the AABB.
		bool IsInside(const Vector3<T>& aPosition) const
		{
			if (aPosition.x <= myMax.x && aPosition.x >= myMin.x)
			{
				if (aPosition.y <= myMax.y && aPosition.y >= myMin.y)
				{
					if (aPosition.z <= myMax.z && aPosition.z >= myMin.z)
					{
						return true;
					}
				}
			}

				return false;
		}
		Vector3<T> GetMin() const
		{
			return myMin;
		}
		Vector3<T> GetMax() const
		{
			return myMax;
		}
	private:
		Vector3<T> myMin;
		Vector3<T> myMax;
	};
}
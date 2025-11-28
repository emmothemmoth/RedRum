#pragma once
#include "Plane.hpp"
#include <vector>

namespace CommonUtilities
{
	template <class T> 
	class PlaneVolume
	{
	public:
		// Default constructor: empty PlaneVolume.
		PlaneVolume()
		{
			myPlaneVolume = {};
		}

		// Constructor taking a list of Plane that makes up the PlaneVolume.
		PlaneVolume(const std::vector<Plane<T>>& aPlaneList)
		{
			myPlaneVolume = aPlaneList;
		}

		// Add a Plane to the PlaneVolume.
		void AddPlane(const Plane<T>& aPlane)
		{
			myPlaneVolume.push_back(aPlane);
		}

		// Returns whether a point is inside the PlaneVolume: it is inside when the point is on the
			// plane or on the side the normal is pointing away from for all the planes in the PlaneVolume.
		bool IsInside(const Vector3<T>& aPosition) const
		{
			bool isInside = true;
			for (int index = 0; index < myPlaneVolume.size(); ++index)
			{
				if (!myPlaneVolume[index].IsInside(aPosition))
				{
					isInside = false;
				}
			}
			return isInside;
		}

		std::vector<Plane<T>> myPlaneVolume = {};
	};
}

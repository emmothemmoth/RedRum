#pragma once
#include <vector>
#include "Line.hpp"
#include "Vector2.hpp"

namespace CommonUtilities
{
	template <class T>
	class LineVolume
	{
	public:
		// Default constructor: empty LineVolume.
		LineVolume()
		{
			myLineVolume = {};
		}

		// Constructor taking a list of Line that makes up the LineVolume.
		LineVolume(const std::vector<Line<T>>& aLineList)
		{
			for (int index = 0; index < aLineList.size(); ++index)
			{
				myLineVolume.push_back(aLineList[index]);
			}
		}

		// Add a Line to the LineVolume.
		void AddLine(const Line<T>& aLine)
		{
			myLineVolume.push_back(aLine);
		}

		// Returns whether a point is inside the LineVolume: it is inside when the point is
		// on the line or on the side the normal is pointing away from for all the lines in
		// the LineVolume.
		bool IsInside(const Vector2<T>& aPosition) const
		{
			bool isInside = true;
			for (int index = 0; index < myLineVolume.size(); ++index)
			{
				if (!myLineVolume[index].IsInside(aPosition))
				{
					isInside = false;
				}
			}
			return isInside;
		}
		std::vector<Line<T>> myLineVolume;
	};
}

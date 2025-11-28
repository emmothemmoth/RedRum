#pragma once
#include "Vector2.hpp"


namespace CommonUtilities
{
	template <class T>
	class Line
	{
	public:
		// Default constructor: there is no line, the normal is the zero vector.
		Line()
		{
			myPoint0.x = 0;
			myPoint0.y = 0;
			myPoint1.x = 0;
			myPoint1.y = 0;
		}

		// Copy constructor.
		Line(const Line <T>& aLine)
		{
			myPoint0 = aLine.myPoint0;
			myPoint1 = aLine.myPoint1;
		}

		// Constructor that takes two points that define the line, the direction is aPoint1 - aPoint0.
		Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
		{
			myPoint0 = aPoint0;
			myPoint1 = aPoint1;
		}

		// Init the line with two points, the same as the constructor above.
		void InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
		{
			myPoint0 = aPoint0;
			myPoint0 = aPoint0;
			myPoint1 = aPoint1;
		}

		// Init the line with a point and a direction.
		void InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection)
		{
			myPoint0 = aPoint;
			myPoint1 = myPoint0 + aDirection;
		}

		// Returns whether a point is inside the line: it is inside when the point is on the 
		//line or on the side the normal is pointing away from.
		bool IsInside(const Vector2<T>& aPosition) const
		{
			Vector2<T> aPosVec = aPosition - myPoint0;
			Vector2<T> normal = this->GetNormal();
			T dotProduct = aPosVec.Dot(normal);

			if (dotProduct < 0.000001f)
			{
				return true;
			}
			else
			{
				return false;
			}

		}

		// Returns the direction of the line.
		const Vector2<T>& GetDirection() const
		{
			Vector2<T> direction;
			direction = myPoint1 - myPoint0;
			direction.Normalize();
			return direction;
		}

		// Returns the normal of the line, which is (-direction.y, direction.x).
		const Vector2<T> GetNormal() const
		{
			Vector2<T> direction = myPoint1 - myPoint0;
			Vector2<T> normal;
			normal.x = direction.y * -1;
			normal.y = direction.x;
			normal.Normalize();
			return normal;
		}
		Vector2<T> myPoint0;
		Vector2<T> myPoint1;

	};
}

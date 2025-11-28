#pragma once
#include <assert.h>
#include "Matrix.hpp"

namespace CommonUtilities
{
	template <typename T>
	T Min(const T aFirstValue, const T aSecondValue)
	{
		if (aFirstValue < aSecondValue)
		{
			return aFirstValue;
		}
		else if (aSecondValue < aFirstValue)
		{
			return aSecondValue;
		}
		else
		{
			return 0;
		}

	}


	template <typename T>
	T Max(const T aFirstValue, const T aSecondValue)
	{
		if (aFirstValue > aSecondValue)
		{
			return aFirstValue;
		}
		else if (aSecondValue > aFirstValue)
		{
			return aSecondValue;
		}
		else
		{
			return 0;
		}
	}

	template <typename T>
	T Abs(const T aValue)
	{
		if (aValue < 0)
		{
			return (aValue * -1);
		}
		else
		{
			return aValue;
		}
	}


	template <typename T>
	T Clamp(const T aValue, const T aMinValue, const T aMaxValue)
	{
		assert(aMinValue <= aMaxValue);
		
		if (aValue < aMinValue)
		{
			return aMinValue;
		}
		else if (aValue > aMaxValue)
		{
			return aMaxValue;
		}
		else
		{
			return aValue;
		}
	}

	template <typename T>
	T Lerp(T a, T b, float c)
	{
		return a * (1.0f - c) + (b * c);
	}

	template <typename T>
	void Swap(T& aFirstValue, T& aSecondValue)
	{
		T temp = aFirstValue;
		aFirstValue = aSecondValue;
		aSecondValue = temp;
	}

	template <typename T, typename U>
	void TGAMatrix4ToCUMatrix4(const T& aTGAMatrix, U& aCUMatrix)
	{
		aCUMatrix(1, 1) = aTGAMatrix.m11;
		aCUMatrix(1, 2) = aTGAMatrix.m12;
		aCUMatrix(1, 3) = aTGAMatrix.m13;
		aCUMatrix(1, 4) = aTGAMatrix.m14;
		aCUMatrix(2, 1) = aTGAMatrix.m21;
		aCUMatrix(2, 2) = aTGAMatrix.m22;
		aCUMatrix(2, 3) = aTGAMatrix.m23;
		aCUMatrix(2, 4) = aTGAMatrix.m24;
		aCUMatrix(3, 1) = aTGAMatrix.m31;
		aCUMatrix(3, 2) = aTGAMatrix.m32;
		aCUMatrix(3, 3) = aTGAMatrix.m33;
		aCUMatrix(3, 4) = aTGAMatrix.m34;
		aCUMatrix(4, 1) = aTGAMatrix.m41;
		aCUMatrix(4, 2) = aTGAMatrix.m42;
		aCUMatrix(4, 3) = aTGAMatrix.m43;
		aCUMatrix(4, 4) = aTGAMatrix.m44;
	}

	template <typename T, typename U>
	void TGAFBXMatrix4ToCUMatrix4(const T& aTGAFBXMatrix, U& aCUMatrix)
	{
		aCUMatrix(1, 1) = aTGAFBXMatrix.m11;
		aCUMatrix(1, 2) = aTGAFBXMatrix.m12;
		aCUMatrix(1, 3) = aTGAFBXMatrix.m13;
		aCUMatrix(1, 4) = aTGAFBXMatrix.m14;
		aCUMatrix(2, 1) = aTGAFBXMatrix.m21;
		aCUMatrix(2, 2) = aTGAFBXMatrix.m22;
		aCUMatrix(2, 3) = aTGAFBXMatrix.m23;
		aCUMatrix(2, 4) = aTGAFBXMatrix.m24;
		aCUMatrix(3, 1) = aTGAFBXMatrix.m31;
		aCUMatrix(3, 2) = aTGAFBXMatrix.m32;
		aCUMatrix(3, 3) = aTGAFBXMatrix.m33;
		aCUMatrix(3, 4) = aTGAFBXMatrix.m34;
		aCUMatrix(4, 1) = aTGAFBXMatrix.m41;
		aCUMatrix(4, 2) = aTGAFBXMatrix.m42;
		aCUMatrix(4, 3) = aTGAFBXMatrix.m43;
		aCUMatrix(4, 4) = aTGAFBXMatrix.m44;
	}
}

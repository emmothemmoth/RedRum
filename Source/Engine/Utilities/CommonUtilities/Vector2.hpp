//#pragma message(__FILE__ "was compiled")
#pragma once
#include <cassert>

namespace CommonUtilities
{
	template <class T>
	class Vector2
	{
	public:
		T x;
		T y;

		//Creates a null-vector
		Vector2<T>()
		{
			x = 0;
			y = 0;
		}

		//Creates a vector (aX, aY, aZ)
		Vector2<T>(const T& aX, const T& aY)
		{
			x = aX;
			y = aY;
		}

		//Copy constructor (compiler generated)
		Vector2<T>(const Vector2<T>& aVector) = default;


		//Assignment operator (compiler generated)
		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;

		//Destructor (compiler generated)
		~Vector2<T>(){}

		//Implicit cast operator to any datatype, return initializer list of x, y,
		template <class U> operator U() const
		{
			return { x, y };
		}

		//Returns a negated copy of this vector
		Vector2<T> operator-() const
		{
			Vector2<T> negatedVector;
			negatedVector.x = x * -1;
			negatedVector.y = y * -1;
			return negatedVector;
		}

		//Returns the squared length of the vector
		T LengthSqr() const
		{
			T sqrLength = static_cast<T> ((x * x) + (y * y));
			return sqrLength;
		}

		//Returns the length of the vector
		T Length() const
		{
			T length = static_cast<T> (sqrt((x * x) + (y * y)));
			return length;
		}

		//Returns a normalized copy of this vector
		Vector2<T> GetNormalized() const
		{
			Vector2<T> normalizedVectorCopy;
			T length = static_cast<T> (sqrt((x * x) + (y * y)));
			assert(length != 0);
			T multiplierLength = 1 / length;
			normalizedVectorCopy.x = x * multiplierLength;
			normalizedVectorCopy.y = y * multiplierLength;
			return normalizedVectorCopy;

		}

		//Normalizes the vector
		void Normalize()
		{
			T length = static_cast<T> (sqrt((x * x) + (y * y)));
			assert(length != 0);
			T multiplierLength = 1 / length;
			x = (x * multiplierLength);
			y = (y * multiplierLength);
		}

		//Returns the dot product of this and aVector
		T Dot(const Vector2<T>& aVector) const
		{
			return (x * aVector.x) + (y * aVector.y);
		}

	};
		//Returns the vector sum of aVector0 and aVector1
		template <class T> Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
		{
			Vector2<T> vectorSum;
			vectorSum.x = aVector0.x + aVector1.x;
			vectorSum.y = aVector0.y + aVector1.y;
			return vectorSum;
		}

		//Returns the vector difference of aVector0 and aVector1
		template <class T> Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
		{
			Vector2<T> vectorSum;
			vectorSum.x = aVector0.x - aVector1.x;
			vectorSum.y = aVector0.y - aVector1.y;
			return vectorSum;
		}

		//Returns the vector aVector multiplied by the scalar aScalar
		template <class T> Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
		{
			Vector2<T> aMultVector;
			aMultVector.x = aVector.x;
			aMultVector.y = aVector.y;
			aMultVector.x *= aScalar;
			aMultVector.y *= aScalar;
			return aMultVector;
		}

		//Returns the vector aVector multiplied by the scalar aScalar
		template <class T> Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
		{
			Vector2<T> aScaledVector;
			aScaledVector.x = aVector.x * aScalar;
			aScaledVector.y = aVector.y * aScalar;
			return aScaledVector;
		}

		//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
		template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
		{
			assert((aScalar != 0) && "Division with 0");
			if (aScalar == 0)
			{
				aVector.x = 0;
				aVector.y = 0;
			}
			else
			{
				Vector2<T> aDivVector;
				T aDivScalar = static_cast<T> (1 / aScalar);
				aDivVector.x = aVector.x * aDivScalar;
				aDivVector.y = aVector.y * aDivScalar;
				return aDivVector;
			}
			
		}

		//Equivalent to setting aVector0 to (aVector0 + aVector1)
		template <class T> void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
		{
			aVector0.x = aVector0.x + aVector1.x;
			aVector0.y = aVector0.y + aVector1.y;
		}

		//Equivalent to setting aVector0 to (aVector0 - aVector1)
		template <class T> void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
		{
			aVector0.x = aVector0.x - aVector1.x;
			aVector0.y = aVector0.y - aVector1.y;
		}

		//Equivalent to setting aVector to (aVector * aScalar)
		template <class T> void operator*=(Vector2<T>& aVector, const T& aScalar)
		{
			aVector.x = aVector.x * aScalar;
			aVector.y = aVector.y * aScalar;
		}

		//Equivalent to setting aVector to (aVector / aScalar)
		template <class T> void operator/=(Vector2<T>& aVector, const T& aScalar)
		{
			assert((aScalar != 0) && "Division with 0");
			if (aScalar == 0)
			{
				aVector.x = 0;
				aVector.y = 0;
			}
			else
			{
				T aDivScalar = static_cast<T> (1 / aScalar);
				aVector.x = aVector.x * aDivScalar;
				aVector.y = aVector.y * aDivScalar;
			}
		}
		typedef Vector2<float> Vector2f;
}
namespace CU = CommonUtilities;

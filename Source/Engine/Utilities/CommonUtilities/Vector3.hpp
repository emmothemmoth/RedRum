//#pragma message(__FILE__ "was compiled")
#pragma once
#include <cassert>
#include <cmath>
#include "Vector4.hpp"

namespace CommonUtilities
{
	template <class T>
	class Vector3
	{
	public:
		T x;
		T y;
		T z;

		//Creates a null-vector
		Vector3<T>()
		{
			x = 0;
			y = 0;
			z = 0;
		}

		//Creates a vector (aX, aY, aZ)
		Vector3<T>(const T& aX, const T& aY, const T& aZ)
		{
			x = aX;
			y = aY;
			z = aZ;
		}

		//Copy constructor (compiler generated)
		Vector3<T>(const Vector3<T>& aVector) = default;

		//Assignment operator (compiler generated)
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;

		//Destructor (compiler generated)
		~Vector3<T>() = default;

		//Implicit cast operator to any datatype, return initializer list of x, y, z
		/*template <class U> operator U() const
		{
			return { x, y, z };
		}*/

		//Returns a negated copy of this vector
		Vector3<T> operator-() const
		{
			Vector3<T> negatedVector;
			negatedVector.x = x * -1;
			negatedVector.y = y * -1;
			negatedVector.z = z * -1;
			return negatedVector;
		}

		//Returns the squared length of the vector
		T LengthSqr() const
		{
			T sqrLength = static_cast<T> ((x * x) + (y * y) + (z * z));
			return sqrLength;
		}

		//Returns the length of the vector
		T Length() const
		{
			T length = static_cast<T> (sqrt((x * x) + (y * y) + (z * z)));
			return length;
		}


		//Returns a vector4 copy of input
		static Vector4<T> GetVec4(const Vector3<T>& aVector)
		{
			Vector4<T> vector;
			vector.x = aVector.x;
			vector.y = aVector.x;
			vector.z = aVector.x;
			vector.w = 0;
			return vector;
		}

		//Returns a normalized copy of this vector
		Vector3<T> GetNormalized() const
		{
			T length = Length();
			if (length != 0)
			{
				T lengthInverse = 1 / length;
				return Vector3<T>(x * lengthInverse, y * lengthInverse, z * lengthInverse);
			}
			return Vector3<T>();
		}

		//Normalizes the vector
		void Normalize()
		{
			T length = static_cast<T> (sqrt((x * x) + (y * y) + (z * z)));
			assert(length != 0);
			T multiplierLength = 1 / length;
			x = (x * multiplierLength);
			y = (y * multiplierLength);
			z = (z * multiplierLength);
		}

		//Returns the dot product of this and aVector
		T Dot(const Vector3<T>& aVector) const
		{
			return (x * aVector.x) + (y * aVector.y) + (z * aVector.z);
		}

		//Returns the cross product of this and aVector
		Vector3<T> Cross(const Vector3<T>& aVector) const
		{
			Vector3<T> crossProduct;
			crossProduct.x = ((y * aVector.z) - (z * aVector.y));
			crossProduct.y = ((z * aVector.x) - (x * aVector.z));
			crossProduct.z = ((x * aVector.y) - (y * aVector.x));
			return crossProduct;
		}
	};
		//Returns the vector sum of aVector0 and aVector1
		template <class T> Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
		{
			Vector3<T> vectorSum;
			vectorSum.x = aVector0.x + aVector1.x;
			vectorSum.y = aVector0.y + aVector1.y;
			vectorSum.z = aVector0.z + aVector1.z;
			return vectorSum;
		}

		//Returns the vector difference of aVector0 and aVector1
		template <class T> Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
		{
			Vector3<T> vectorSum;
			vectorSum.x = aVector0.x - aVector1.x;
			vectorSum.y = aVector0.y - aVector1.y;
			vectorSum.z = aVector0.z - aVector1.z;
			return vectorSum;
		}

		//Returns the vector aVector multiplied by the scalar aScalar
		template <class T> Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
		{
			Vector3<T> aMultVector;
			aMultVector.x = aVector.x;
			aMultVector.y = aVector.y;
			aMultVector.z = aVector.z;
			aMultVector.x *= aScalar;
			aMultVector.y *= aScalar;
			aMultVector.z *= aScalar;
			return aMultVector;
		}

		//Returns the vector aVector multiplied by the scalar aScalar
		template <class T> Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
		{
			Vector3<T> aMultVector;
			aMultVector.x = aVector.x;
			aMultVector.y = aVector.y;
			aMultVector.z = aVector.z;
			aMultVector.x *= aScalar;
			aMultVector.y *= aScalar;
			aMultVector.z *= aScalar;
			return aMultVector;
		}

		//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
		template <class T> Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
		{
			assert((aScalar != 0) && "Division with 0");
			if (aScalar == 0)
			{
				aVector.x = 0;
				aVector.y = 0;
			}
			Vector3<T> aDivVector;
			T aDivScalar = static_cast<T> (1 / aScalar);
			aDivVector.x = aVector.x * aDivScalar;
			aDivVector.y = aVector.y * aDivScalar;
			aDivVector.z = aVector.z * aDivScalar;
			return aDivVector;
			
		}

		//Equivalent to setting aVector0 to (aVector0 + aVector1)
		template <class T> void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
		{
			aVector0.x = aVector0.x + aVector1.x;
			aVector0.y = aVector0.y + aVector1.y;
			aVector0.z = aVector0.z + aVector1.z;
		}

		//Equivalent to setting aVector0 to (aVector0 - aVector1)
		template <class T> void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
		{
			aVector0.x = aVector0.x - aVector1.x;
			aVector0.y = aVector0.y - aVector1.y;
			aVector0.z = aVector0.z - aVector1.z;
		}

		//Equivalent to setting aVector to (aVector * aScalar)
		template <class T> void operator*=(Vector3<T>& aVector, const T& aScalar)
		{
			aVector.x = aVector.x * aScalar;
			aVector.y = aVector.y * aScalar;
			aVector.z = aVector.z * aScalar;
		}

		//Equivalent to setting aVector to (aVector / aScalar)
		template <class T> void operator/=(Vector3<T>& aVector, const T& aScalar)
		{
			assert((aScalar != 0) && "Division with 0");
			if (aScalar == 0)
			{
				aVector.x = 0;
				aVector.y = 0;
			}
			else
			{
				Vector3<T> aDivVector;
				T aDivScalar = static_cast<T> (1 / aScalar);
				aVector.x = aVector.x * aDivScalar;
				aVector.y = aVector.y * aDivScalar;
				aVector.z = aVector.z * aDivScalar;
			}
			
		}
		typedef Vector3<float> Vector3f;
}
namespace CU = CommonUtilities;

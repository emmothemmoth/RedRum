//#pragma message(__FILE__ "was compiled")
#pragma once
#include <cassert>

namespace CommonUtilities
{
	template <class T>
	class Vector4
	{
	public:
		T x;
		T y;
		T z;
		T w;

		//Creates a null-vector
		Vector4<T>()
		{
			x = 0;
			y = 0;
			z = 0;
			w = 0;
		}

		//Creates a vector (aX, aY, aZ)
		Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW)
		{
			x = aX;
			y = aY;
			z = aZ;
			w = aW;
		}

		//Copy constructor (compiler generated)
		Vector4<T>(const Vector4<T>& aVector) = default;

		//Assignment operator (compiler generated)
		Vector4<T>& operator=(const Vector4<T>& aVector) = default;

		//Destructor (compiler generated)
		~Vector4<T>() {}

		//Implicit cast operator to any datatype, return initializer list of x, y,
		template <class U> operator U() const
		{
			return { x, y ,z ,w };
		}

		//Returns a negated copy of this vector
		Vector4<T> operator-() const
		{
			Vector4<T> negatedVector;
			negatedVector.x = x * -1;
			negatedVector.y = y * -1;
			negatedVector.z = z * -1;
			negatedVector.w = w * -1;
			return negatedVector;
		}

		//Returns the squared length of the vector
		T LengthSqr() const
		{
			T sqrLength = static_cast<T> ((x * x) + (y * y) + (z * z) + (w * w));
			return sqrLength;
		}

		//Returns the length of the vector
		T Length() const
		{
			T length = static_cast<T> (sqrt(x * x + y * y + z * z + w * w));
			return length;
		}

		//Returns a normalized copy of this vector
		Vector4<T> GetNormalized() const
		{
			Vector4<T> normalizedVectorCopy;
			T length = static_cast<T> (sqrt((x * x) + (y * y) + (z * z) + (w * w)));
			assert(length != 0);
			T multiplierLength = 1 / length;
			normalizedVectorCopy.x = x * multiplierLength;
			normalizedVectorCopy.y = y * multiplierLength;
			normalizedVectorCopy.z = z * multiplierLength;
			normalizedVectorCopy.w = w * multiplierLength;
			return normalizedVectorCopy;

		}

		//Normalizes the vector
		void Normalize()
		{
			T length = static_cast<T> (sqrt((x * x) + (y * y) + (z * z) + (w * w)));
			assert(length != 0);
			T multiplierLength = 1 / length;
			x = (x * multiplierLength);
			y = (y * multiplierLength);
			z = (z * multiplierLength);
			w = (w * multiplierLength);
		}

		//Returns the dot product of this and aVector
		T Dot(const Vector4<T>& aVector) const
		{
			return (x * aVector.x) + (y * aVector.y) + (z * aVector.z) + (w * aVector.w);
		}

	};
	//Returns the vector sum of aVector0 and aVector1
	template <class T> Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		Vector4<T> vectorSum;
		vectorSum.x = aVector0.x + aVector1.x;
		vectorSum.y = aVector0.y + aVector1.y;
		vectorSum.z = aVector0.z + aVector1.z;
		vectorSum.w = aVector0.w + aVector1.w;
		return vectorSum;
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T> Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		Vector4<T> vectorSum;
		vectorSum.x = aVector0.x - aVector1.x;
		vectorSum.y = aVector0.y - aVector1.y;
		vectorSum.z = aVector0.z - aVector1.z;
		vectorSum.w = aVector0.w - aVector1.w;
		return vectorSum;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		Vector4<T> aMultVector;
		aMultVector.x = aVector.x;
		aMultVector.y = aVector.y;
		aMultVector.z = aVector.z;
		aMultVector.w = aVector.w;
		aMultVector.x *= aScalar;
		aMultVector.y *= aScalar;
		aMultVector.z *= aScalar;
		aMultVector.w *= aScalar;
		return aMultVector;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
	{
		Vector4<T> aMultVector;
		aMultVector.x = aVector.x;
		aMultVector.y = aVector.y;
		aMultVector.z = aVector.z;
		aMultVector.w = aVector.w;
		aMultVector.x *= aScalar;
		aMultVector.y *= aScalar;
		aMultVector.z *= aScalar;
		aMultVector.w *= aScalar;
		return aMultVector;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
	template <class T> Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
	{
		Vector4<T> aDivVector;
		T aDivScalar = static_cast<T> (1 / aScalar);
		aDivVector.x = aVector.x * aDivScalar;
		aDivVector.y = aVector.y * aDivScalar;
		aDivVector.z = aVector.z * aDivScalar;
		aDivVector.w = aVector.w * aDivScalar;
		return aDivVector;

	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T> void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x = aVector0.x + aVector1.x;
		aVector0.y = aVector0.y + aVector1.y;
		aVector0.z = aVector0.z + aVector1.z;
		aVector0.w = aVector0.w + aVector1.w;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T> void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x = aVector0.x - aVector1.x;
		aVector0.y = aVector0.y - aVector1.y;
		aVector0.z = aVector0.z - aVector1.z;
		aVector0.w = aVector0.w - aVector1.w;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T> void operator*=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.x = aVector.x * aScalar;
		aVector.y = aVector.y * aScalar;
		aVector.z = aVector.z * aScalar;
		aVector.w = aVector.w * aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T> void operator/=(Vector4<T>& aVector, const T& aScalar)
	{
		Vector4<T> aDivVector;
		T aDivScalar = static_cast<T> (1 / aScalar);
		aVector.x = aVector.x * aDivScalar;
		aVector.y = aVector.y * aDivScalar;
		aVector.z = aVector.z * aDivScalar;
		aVector.w = aVector.w * aDivScalar;
	}
	typedef Vector4<float> Vector4f;
}
namespace CU = CommonUtilities;
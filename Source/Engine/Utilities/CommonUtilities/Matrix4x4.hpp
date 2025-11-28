//#pragma message(__FILE__ "was compiled")
#pragma once
#include <array>
#include "Vector4.hpp"
#include "Matrix.hpp"

namespace CommonUtilities
{
	template <class T>
	class Matrix4x4
	{
	public:
		// Creates the identity matrix.
		Matrix4x4<T>()
		{
			myMatrix[0][0] = static_cast<T>(1);
			myMatrix[0][1] = static_cast<T>(0);
			myMatrix[0][2] = static_cast<T>(0);
			myMatrix[0][3] = static_cast<T>(0);
					  
			myMatrix[1][0] = static_cast<T>(0);
			myMatrix[1][1] = static_cast<T>(1);
			myMatrix[1][2] = static_cast<T>(0);
			myMatrix[1][3] = static_cast<T>(0);
					  
			myMatrix[2][0] = static_cast<T>(0);
			myMatrix[2][1] = static_cast<T>(0);
			myMatrix[2][2] = static_cast<T>(1);
			myMatrix[2][3] = static_cast<T>(0);
					  
			myMatrix[3][0] = static_cast<T>(0);
			myMatrix[3][1] = static_cast<T>(0);
			myMatrix[3][2] = static_cast<T>(0);
			myMatrix[3][3] = static_cast<T>(1);
		}

		// Copy Constructor.
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix)
		{
			for (int index = 0; index < myRows; ++index)
			{
				for (int index2 = 0; index2 < myColumns; ++index2)
				{
					myMatrix[index][index2] = aMatrix(index + 1, index2 + 1);
				}
			}
		}

		// () operator for accessing element (row, column) for read/write or read, respectively.
		T& operator()(const int aRow, const int aColumn)
		{
			return myMatrix[aRow - 1][aColumn - 1];
		}

		const T& operator()(const int aRow, const int aColumn) const
		{
			return myMatrix[aRow - 1][aColumn - 1];
		}

		Matrix4x4<T>& operator=(const Matrix4x4<T>& aMatrix) = default;

		Vector4<T> GetRow(const unsigned aRowIndex)
		{
			assert(aRowIndex > 0 && aRowIndex < 5);
			Vector4<T> row;
			row.x = this->myMatrix[aRowIndex-1][0];
			row.y = this->myMatrix[aRowIndex-1][1];
			row.z = this->myMatrix[aRowIndex-1][2];
			row.w = this->myMatrix[aRowIndex-1][3];
			return row;
		}

		void SetRow(const Vector4<T>& aRowVector, const unsigned& aRowIndex)
		{
			assert(aRowIndex > 0 && aRowIndex < 5);
			myMatrix[aRowIndex - 1][0] = aRowVector.x;
			myMatrix[aRowIndex - 1][1] = aRowVector.y;
			myMatrix[aRowIndex - 1][2] = aRowVector.z;
			myMatrix[aRowIndex - 1][3] = aRowVector.w;
		}

		// Static functions for creating rotation matrices.
		static Matrix4x4<T> CreateRotationAroundX(const T aAngleInRadians)
		{
			Matrix4x4<T> rotationMatrix;
			rotationMatrix(1, 1) = static_cast<T>(1);
			rotationMatrix(1, 2) = static_cast<T>(0);
			rotationMatrix(1, 3) = static_cast<T>(0);
			rotationMatrix(1, 4) = static_cast<T>(0);

			rotationMatrix(2, 1) = static_cast<T>(0);
			rotationMatrix(2, 2) = static_cast<T>(cos(aAngleInRadians));
			rotationMatrix(2, 3) = static_cast<T>(sin(aAngleInRadians));
			rotationMatrix(2, 4) = static_cast<T>(0);

			rotationMatrix(3, 1) = static_cast<T>(0);
			rotationMatrix(3, 2) = static_cast<T>(-1 * sin(aAngleInRadians));
			rotationMatrix(3, 3) = static_cast<T>(cos(aAngleInRadians));
			rotationMatrix(3, 4) = static_cast<T>(0);
			
			rotationMatrix(4, 1) = static_cast<T>(0);
			rotationMatrix(4, 2) = static_cast<T>(0);
			rotationMatrix(4, 3) = static_cast<T>(0);
			rotationMatrix(4, 4) = static_cast<T>(1);

			return rotationMatrix;
		}

		static Matrix4x4<T> CreateRotationAroundY(const T aAngleInRadians)
		{
			Matrix4x4<T> rotationMatrix;
			rotationMatrix(1, 1) = static_cast<T>(cos(aAngleInRadians));
			rotationMatrix(1, 2) = static_cast<T>(0);
			rotationMatrix(1, 3) = static_cast<T> (-1 * sin(aAngleInRadians));
			rotationMatrix(1, 4) = static_cast<T>(0);

			rotationMatrix(2, 1) = static_cast<T>(0);
			rotationMatrix(2, 2) = static_cast<T>(1);
			rotationMatrix(2, 3) = static_cast<T>(0);
			rotationMatrix(2, 4) = static_cast<T>(0);

			rotationMatrix(3, 1) = static_cast<T> (sin(aAngleInRadians));
			rotationMatrix(3, 2) = static_cast<T>(0);
			rotationMatrix(3, 3) = static_cast<T> (cos(aAngleInRadians));
			rotationMatrix(3, 4) = static_cast<T>(0);

			rotationMatrix(4, 1) = static_cast<T>(0);
			rotationMatrix(4, 2) = static_cast<T>(0);
			rotationMatrix(4, 3) = static_cast<T>(0);
			rotationMatrix(4, 4) = static_cast<T>(1);


			return rotationMatrix;
		}

		static Matrix4x4<T> CreateRotationAroundZ(const T aAngleInRadians)
		{
			Matrix4x4<T> rotationMatrix;
			rotationMatrix(1, 1) = static_cast<T>(cos(aAngleInRadians));
			rotationMatrix(1, 2) = static_cast<T>(sin(aAngleInRadians));
			rotationMatrix(1, 3) = static_cast<T>(0);
			rotationMatrix(1, 4) = static_cast<T>(0);

			rotationMatrix(2, 1) = static_cast<T>(-1 * sin(aAngleInRadians));
			rotationMatrix(2, 2) = static_cast<T>(cos(aAngleInRadians));
			rotationMatrix(2, 3) = static_cast<T>(0);
			rotationMatrix(2, 4) = static_cast<T>(0);

			rotationMatrix(3, 1) = static_cast<T>(0);
			rotationMatrix(3, 2) = static_cast<T>(0);
			rotationMatrix(3, 3) = static_cast<T>(1);
			rotationMatrix(3, 4) = static_cast<T>(0);
			
			rotationMatrix(4, 1) = static_cast<T>(0);
			rotationMatrix(4, 2) = static_cast<T>(0);
			rotationMatrix(4, 3) = static_cast<T>(0);
			rotationMatrix(4, 4) = static_cast<T>(1);

			return rotationMatrix;
		}

		static Matrix4x4<T> CreateOrthoGraphicProjection(const T aLeftPlane, const T aRightPlane, const T aBottomPlane, const T aTopPlane, const T aNearPlane, const T aFarPlane)
		{
			const float reciprocalWidth = 1.0f / (aRightPlane - aLeftPlane);
			const float reciprocalHeight = 1.0f / (aTopPlane - aBottomPlane);
			const float fRange = 1.0f / (aFarPlane - aNearPlane);

			Matrix4x4<T> matrix;
			matrix(1, 1) = reciprocalWidth + reciprocalWidth;
			matrix(2, 2) = reciprocalHeight + reciprocalHeight;
			matrix(3, 3) = fRange;
			matrix(4,1) = - 1 *(aLeftPlane + aRightPlane) * reciprocalWidth;
			matrix(4,2) = -1 * (aTopPlane + aBottomPlane) * reciprocalHeight;
			matrix(4, 3) = -1 * fRange * aNearPlane;
			matrix(4, 4) = 1;
			return matrix;
		}

		static Matrix4x4<T> CreatePerspectiveProjection(const T aHorizontalFov, const T aFarZ, const T aNearZ, const T aResolutionX, const T aResolutionY)
		{
			Matrix4x4<T> matrix;

			float scaleX = 1 / static_cast<float>(tan(aHorizontalFov * 0.5f));
			float scaleY = (aResolutionX / aResolutionY) * scaleX;
			float planesDiv = aFarZ / (aFarZ - aNearZ);

			matrix(1, 1) = scaleX;
			matrix(1, 2) = 0;
			matrix(1, 3) = 0;
			matrix(1, 4) = 0;
			matrix(2, 1) = 0;
			matrix(2, 2) = scaleY;
			matrix(2, 3) = 0;
			matrix(2, 4) = 0;
			matrix(3, 1) = 0;
			matrix(3, 2) = 0;
			matrix(3, 3) = planesDiv;
			matrix(3, 4) = 1;
			matrix(4, 1) = 0;
			matrix(4, 2) = 0;
			matrix(4, 3) = (-1 * planesDiv) * aNearZ;
			matrix(4, 4) = 0;
			return matrix;
		}


		static Matrix4x4<T> LookAt(const Vector3<T>& aPosition, const Vector3<T>& aTarget, const Vector3<T> aGlobalUP)
		{
			Vector3<T> zAxis = (aTarget - aPosition).GetNormalized();
			Vector3<T> xAxis = aGlobalUP.Cross(zAxis).GetNormalized();
			Vector3<T> yAxis = zAxis.Cross(xAxis).GetNormalized();

			Matrix4x4<T> result;
			result(1, 1) = xAxis.x;
			result(1, 2) = xAxis.y;
			result(1, 3) = xAxis.z;
			result(2, 1) = yAxis.x;
			result(2, 2) = yAxis.y;
			result(2, 3) = yAxis.z;
			result(3, 1) = zAxis.x;
			result(3, 2) = zAxis.y;
			result(3, 3) = zAxis.z;
			result(4, 1) = aPosition.x;
			result(4, 2) = aPosition.y;
			result(4, 3) = aPosition.z;
			return result;
		}

		// Static function for creating a transpose of a matrix.
		static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose)
		{
			static int statRows = 4;
			static int statColumns = 4;
			Matrix4x4<T> transposedMatrix;
			for (size_t index = 0; index < statRows; ++index)
			{
				for (size_t index2 = 0; index2 < statColumns; ++index2)
				{
					transposedMatrix.myMatrix[index2][index] = aMatrixToTranspose.myMatrix[index][index2];
				}
			}
			return transposedMatrix;
		}

		Matrix4x4<T> GetInverse() const
		{
			Matrix4x4<T> outMatrix;
			T inv[4][4];
			T det;

			inv[0][0] =
				myMatrix[1][1] * myMatrix[2][2] * myMatrix[3][3] -
				myMatrix[1][1] * myMatrix[2][3] * myMatrix[3][2] -
				myMatrix[2][1] * myMatrix[1][2] * myMatrix[3][3] +
				myMatrix[2][1] * myMatrix[1][3] * myMatrix[3][2] +
				myMatrix[3][1] * myMatrix[1][2] * myMatrix[2][3] -
				myMatrix[3][1] * myMatrix[1][3] * myMatrix[2][2];

			inv[1][0] =
				-myMatrix[1][0] * myMatrix[2][2] * myMatrix[3][3] +
				myMatrix[1][0] * myMatrix[2][3] * myMatrix[3][2] +
				myMatrix[2][0] * myMatrix[1][2] * myMatrix[3][3] -
				myMatrix[2][0] * myMatrix[1][3] * myMatrix[3][2] -
				myMatrix[3][0] * myMatrix[1][2] * myMatrix[2][3] +
				myMatrix[3][0] * myMatrix[1][3] * myMatrix[2][2];


			inv[2][0] =
				myMatrix[1][0] * myMatrix[2][1] * myMatrix[3][3] -
				myMatrix[1][0] * myMatrix[2][3] * myMatrix[3][1] -
				myMatrix[2][0] * myMatrix[1][1] * myMatrix[3][3] +
				myMatrix[2][0] * myMatrix[1][3] * myMatrix[3][1] +
				myMatrix[3][0] * myMatrix[1][1] * myMatrix[2][3] -
				myMatrix[3][0] * myMatrix[1][3] * myMatrix[2][1];


			inv[3][0] =
				-myMatrix[1][0] * myMatrix[2][1] * myMatrix[3][2] +
				myMatrix[1][0] * myMatrix[2][2] * myMatrix[3][1] +
				myMatrix[2][0] * myMatrix[1][1] * myMatrix[3][2] -
				myMatrix[2][0] * myMatrix[1][2] * myMatrix[3][1] -
				myMatrix[3][0] * myMatrix[1][1] * myMatrix[2][2] +
				myMatrix[3][0] * myMatrix[1][2] * myMatrix[2][1];

			inv[0][1] =
				-myMatrix[0][1] * myMatrix[2][2] * myMatrix[3][3] +
				myMatrix[0][1] * myMatrix[2][3] * myMatrix[3][2] +
				myMatrix[2][1] * myMatrix[0][2] * myMatrix[3][3] -
				myMatrix[2][1] * myMatrix[0][3] * myMatrix[3][2] -
				myMatrix[3][1] * myMatrix[0][2] * myMatrix[2][3] +
				myMatrix[3][1] * myMatrix[0][3] * myMatrix[2][2];

			inv[1][1] =
				myMatrix[0][0] * myMatrix[2][2] * myMatrix[3][3] -
				myMatrix[0][0] * myMatrix[2][3] * myMatrix[3][2] -
				myMatrix[2][0] * myMatrix[0][2] * myMatrix[3][3] +
				myMatrix[2][0] * myMatrix[0][3] * myMatrix[3][2] +
				myMatrix[3][0] * myMatrix[0][2] * myMatrix[2][3] -
				myMatrix[3][0] * myMatrix[0][3] * myMatrix[2][2];

			inv[2][1] =
				-myMatrix[0][0] * myMatrix[2][1] * myMatrix[3][3] +
				myMatrix[0][0] * myMatrix[2][3] * myMatrix[3][1] +
				myMatrix[2][0] * myMatrix[0][1] * myMatrix[3][3] -
				myMatrix[2][0] * myMatrix[0][3] * myMatrix[3][1] -
				myMatrix[3][0] * myMatrix[0][1] * myMatrix[2][3] +
				myMatrix[3][0] * myMatrix[0][3] * myMatrix[2][1];

			inv[3][1] =
				myMatrix[0][0] * myMatrix[2][1] * myMatrix[3][2] -
				myMatrix[0][0] * myMatrix[2][2] * myMatrix[3][1] -
				myMatrix[2][0] * myMatrix[0][1] * myMatrix[3][2] +
				myMatrix[2][0] * myMatrix[0][2] * myMatrix[3][1] +
				myMatrix[3][0] * myMatrix[0][1] * myMatrix[2][2] -
				myMatrix[3][0] * myMatrix[0][2] * myMatrix[2][1];

			inv[0][2] =
				myMatrix[0][1] * myMatrix[1][2] * myMatrix[3][3] -
				myMatrix[0][1] * myMatrix[1][3] * myMatrix[3][2] -
				myMatrix[1][1] * myMatrix[0][2] * myMatrix[3][3] +
				myMatrix[1][1] * myMatrix[0][3] * myMatrix[3][2] +
				myMatrix[3][1] * myMatrix[0][2] * myMatrix[1][3] -
				myMatrix[3][1] * myMatrix[0][3] * myMatrix[1][2];

			inv[1][2] =
				-myMatrix[0][0] * myMatrix[1][2] * myMatrix[3][3] +
				myMatrix[0][0] * myMatrix[1][3] * myMatrix[3][2] +
				myMatrix[1][0] * myMatrix[0][2] * myMatrix[3][3] -
				myMatrix[1][0] * myMatrix[0][3] * myMatrix[3][2] -
				myMatrix[3][0] * myMatrix[0][2] * myMatrix[1][3] +
				myMatrix[3][0] * myMatrix[0][3] * myMatrix[1][2];

			inv[2][2] =
				myMatrix[0][0] * myMatrix[1][1] * myMatrix[3][3] -
				myMatrix[0][0] * myMatrix[1][3] * myMatrix[3][1] -
				myMatrix[1][0] * myMatrix[0][1] * myMatrix[3][3] +
				myMatrix[1][0] * myMatrix[0][3] * myMatrix[3][1] +
				myMatrix[3][0] * myMatrix[0][1] * myMatrix[1][3] -
				myMatrix[3][0] * myMatrix[0][3] * myMatrix[1][1];

			inv[3][2] =
				-myMatrix[0][0] * myMatrix[1][1] * myMatrix[3][2] +
				myMatrix[0][0] * myMatrix[1][2] * myMatrix[3][1] +
				myMatrix[1][0] * myMatrix[0][1] * myMatrix[3][2] -
				myMatrix[1][0] * myMatrix[0][2] * myMatrix[3][1] -
				myMatrix[3][0] * myMatrix[0][1] * myMatrix[1][2] +
				myMatrix[3][0] * myMatrix[0][2] * myMatrix[1][1];

			inv[0][3] =
				-myMatrix[0][1] * myMatrix[1][2] * myMatrix[2][3] +
				myMatrix[0][1] * myMatrix[1][3] * myMatrix[2][2] +
				myMatrix[1][1] * myMatrix[0][2] * myMatrix[2][3] -
				myMatrix[1][1] * myMatrix[0][3] * myMatrix[2][2] -
				myMatrix[2][1] * myMatrix[0][2] * myMatrix[1][3] +
				myMatrix[2][1] * myMatrix[0][3] * myMatrix[1][2];

			inv[1][3] =
				myMatrix[0][0] * myMatrix[1][2] * myMatrix[2][3] -
				myMatrix[0][0] * myMatrix[1][3] * myMatrix[2][2] -
				myMatrix[1][0] * myMatrix[0][2] * myMatrix[2][3] +
				myMatrix[1][0] * myMatrix[0][3] * myMatrix[2][2] +
				myMatrix[2][0] * myMatrix[0][2] * myMatrix[1][3] -
				myMatrix[2][0] * myMatrix[0][3] * myMatrix[1][2];

			inv[2][3] =
				-myMatrix[0][0] * myMatrix[1][1] * myMatrix[2][3] +
				myMatrix[0][0] * myMatrix[1][3] * myMatrix[2][1] +
				myMatrix[1][0] * myMatrix[0][1] * myMatrix[2][3] -
				myMatrix[1][0] * myMatrix[0][3] * myMatrix[2][1] -
				myMatrix[2][0] * myMatrix[0][1] * myMatrix[1][3] +
				myMatrix[2][0] * myMatrix[0][3] * myMatrix[1][1];

			inv[3][3] =
				myMatrix[0][0] * myMatrix[1][1] * myMatrix[2][2] -
				myMatrix[0][0] * myMatrix[1][2] * myMatrix[2][1] -
				myMatrix[1][0] * myMatrix[0][1] * myMatrix[2][2] +
				myMatrix[1][0] * myMatrix[0][2] * myMatrix[2][1] +
				myMatrix[2][0] * myMatrix[0][1] * myMatrix[1][2] -
				myMatrix[2][0] * myMatrix[0][2] * myMatrix[1][1];

			det = myMatrix[0][0] * inv[0][0] + myMatrix[0][1] * inv[1][0] + myMatrix[0][2] * inv[2][0] + myMatrix[0][3] * inv[3][0];

			if (det == 0)
			{
				return outMatrix;
			}

			det = static_cast<T>(1) / det;

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					outMatrix.myMatrix[i][j] = inv[i][j] * det;
				}
			}

			return outMatrix;
		}

		// Assumes aTransform is made up of nothing but rotations and translations.
		static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aTransform)
		{
			//Create rotationInverse by transposing the transform
			Matrix4x4<T> rotation = aTransform;
			Matrix4x4<T> transposedRotation = Matrix4x4<T>::Transpose(rotation);

			//Clear transform
			transposedRotation(1, 4) = static_cast<T> (0);
			transposedRotation(2, 4) = static_cast<T> (0);
			transposedRotation(3, 4) = static_cast<T> (0);

			//Create translationInverse as new indentityMatrix and negate the transform (row 4)
			Vector3<T> translationVector;
			translationVector.x = aTransform(4, 1) * -1;
			translationVector.y = aTransform(4, 2) * -1;
			translationVector.z = aTransform(4, 3) * -1;
		
			//Copy 3x3 part of rotationsinverse and multiply with negated translationVector
			Matrix3x3<T> rotationInverse(transposedRotation);
			Vector3<T> inversedVector = translationVector * rotationInverse;

			Matrix4x4<T> fastInverse;
			for (size_t index = 0; index < 3; ++index)
			{
				for (size_t index2 = 0; index2 < 3; ++index2)
				{
					fastInverse.myMatrix[index][index2] = transposedRotation.myMatrix[index][index2];
				}
			}
			fastInverse(4, 1) = inversedVector.x;
			fastInverse(4, 2) = inversedVector.y;
			fastInverse(4, 3) = inversedVector.z;
			fastInverse(4, 4) = static_cast<T> (1);

			//Create fastInverse by multiplying inverses of translation * rotation. OBS! Order matters!

			return fastInverse;

		}


		static const size_t myRows = 4;
		static const size_t myColumns = 4;
		std::array<std::array<T, 4>, 4> myMatrix;

	};

	template <class T> Matrix4x4<T> operator+(const Matrix4x4<T>& aMatrix, const Matrix4x4<T>& anotherMatrix)
	{
		assert(aMatrix.myRows == anotherMatrix.myRows && aMatrix.myColumns == anotherMatrix.myColumns);
		Matrix4x4<T> aMatrixSum;
		for (size_t index = 0; index < aMatrix.myRows; ++index)
		{
			for (size_t index2 = 0; index2 < aMatrix.myColumns; ++index2)
			{
				aMatrixSum.myMatrix[index][index2] = aMatrix.myMatrix[index][index2] + anotherMatrix.myMatrix[index][index2];
			}
		}
		return aMatrixSum;

	}

	template <class T> void operator+=(Matrix4x4<T>& aMatrix, const Matrix4x4<T>& anotherMatrix)
	{
		assert(aMatrix.myRows == anotherMatrix.myRows && aMatrix.myColumns == anotherMatrix.myColumns);

		for (size_t index = 0; index < aMatrix.myRows; ++index)
		{
			for (size_t index2 = 0; index2 < aMatrix.myColumns; ++index2)
			{
				aMatrix.myMatrix[index][index2] = aMatrix.myMatrix[index][index2] + anotherMatrix.myMatrix[index][index2];
			}
		}

	}

	template <class T> Matrix4x4<T> operator-(const Matrix4x4<T>& aMatrix, const Matrix4x4<T>& anotherMatrix)
	{
		assert(aMatrix.myRows == anotherMatrix.myRows && aMatrix.myColumns == anotherMatrix.myColumns);

		Matrix4x4<T> aMatrixSum;
		for (size_t index = 0; index < aMatrix.myRows; ++index)
		{
			for (size_t index2 = 0; index2 < aMatrix.myColumns; ++index2)
			{
				aMatrixSum.myMatrix[index][index2] = aMatrix.myMatrix[index][index2] - anotherMatrix.myMatrix[index][index2];
			}
		}
		return aMatrixSum;
	}

	template <class T> void operator-=(Matrix4x4<T>& aMatrix, const Matrix4x4<T>& anotherMatrix)
	{
		assert(aMatrix.myRows == anotherMatrix.myRows && aMatrix.myColumns == anotherMatrix.myColumns);

		for (size_t index = 0; index < aMatrix.myRows; ++index)
		{
			for (size_t index2 = 0; index2 < aMatrix.myColumns; ++index2)
			{
				aMatrix.myMatrix[index][index2] = aMatrix.myMatrix[index][index2] - anotherMatrix.myMatrix[index][index2];
			}
		}
	}


	template <class T> Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix, const Matrix4x4<T>& anotherMatrix)
	{
		assert(aMatrix.myRows == anotherMatrix.myColumns);

		Matrix4x4<T> matrixProduct;

		matrixProduct.myMatrix[0][0] = aMatrix.myMatrix[0][0] * anotherMatrix.myMatrix[0][0] + aMatrix.myMatrix[0][1] * anotherMatrix.myMatrix[1][0] + aMatrix.myMatrix[0][2] * anotherMatrix.myMatrix[2][0] + aMatrix.myMatrix[0][3] * anotherMatrix.myMatrix[3][0];
		matrixProduct.myMatrix[0][1] = aMatrix.myMatrix[0][0] * anotherMatrix.myMatrix[0][1] + aMatrix.myMatrix[0][1] * anotherMatrix.myMatrix[1][1] + aMatrix.myMatrix[0][2] * anotherMatrix.myMatrix[2][1] + aMatrix.myMatrix[0][3] * anotherMatrix.myMatrix[3][1];
		matrixProduct.myMatrix[0][2] = aMatrix.myMatrix[0][0] * anotherMatrix.myMatrix[0][2] + aMatrix.myMatrix[0][1] * anotherMatrix.myMatrix[1][2] + aMatrix.myMatrix[0][2] * anotherMatrix.myMatrix[2][2] + aMatrix.myMatrix[0][3] * anotherMatrix.myMatrix[3][2];
		matrixProduct.myMatrix[0][3] = aMatrix.myMatrix[0][0] * anotherMatrix.myMatrix[0][3] + aMatrix.myMatrix[0][1] * anotherMatrix.myMatrix[1][3] + aMatrix.myMatrix[0][2] * anotherMatrix.myMatrix[2][3] + aMatrix.myMatrix[0][3] * anotherMatrix.myMatrix[3][3];
		matrixProduct.myMatrix[1][0] = aMatrix.myMatrix[1][0] * anotherMatrix.myMatrix[0][0] + aMatrix.myMatrix[1][1] * anotherMatrix.myMatrix[1][0] + aMatrix.myMatrix[1][2] * anotherMatrix.myMatrix[2][0] + aMatrix.myMatrix[1][3] * anotherMatrix.myMatrix[3][0];
		matrixProduct.myMatrix[1][1] = aMatrix.myMatrix[1][0] * anotherMatrix.myMatrix[0][1] + aMatrix.myMatrix[1][1] * anotherMatrix.myMatrix[1][1] + aMatrix.myMatrix[1][2] * anotherMatrix.myMatrix[2][1] + aMatrix.myMatrix[1][3] * anotherMatrix.myMatrix[3][1];
		matrixProduct.myMatrix[1][2] = aMatrix.myMatrix[1][0] * anotherMatrix.myMatrix[0][2] + aMatrix.myMatrix[1][1] * anotherMatrix.myMatrix[1][2] + aMatrix.myMatrix[1][2] * anotherMatrix.myMatrix[2][2] + aMatrix.myMatrix[1][3] * anotherMatrix.myMatrix[3][2];
		matrixProduct.myMatrix[1][3] = aMatrix.myMatrix[1][0] * anotherMatrix.myMatrix[0][3] + aMatrix.myMatrix[1][1] * anotherMatrix.myMatrix[1][3] + aMatrix.myMatrix[1][2] * anotherMatrix.myMatrix[2][3] + aMatrix.myMatrix[1][3] * anotherMatrix.myMatrix[3][3];					 
		matrixProduct.myMatrix[2][0] = aMatrix.myMatrix[2][0] * anotherMatrix.myMatrix[0][0] + aMatrix.myMatrix[2][1] * anotherMatrix.myMatrix[1][0] + aMatrix.myMatrix[2][2] * anotherMatrix.myMatrix[2][0] + aMatrix.myMatrix[2][3] * anotherMatrix.myMatrix[3][0];
		matrixProduct.myMatrix[2][1] = aMatrix.myMatrix[2][0] * anotherMatrix.myMatrix[0][1] + aMatrix.myMatrix[2][1] * anotherMatrix.myMatrix[1][1] + aMatrix.myMatrix[2][2] * anotherMatrix.myMatrix[2][1] + aMatrix.myMatrix[2][3] * anotherMatrix.myMatrix[3][1];
		matrixProduct.myMatrix[2][2] = aMatrix.myMatrix[2][0] * anotherMatrix.myMatrix[0][2] + aMatrix.myMatrix[2][1] * anotherMatrix.myMatrix[1][2] + aMatrix.myMatrix[2][2] * anotherMatrix.myMatrix[2][2] + aMatrix.myMatrix[2][3] * anotherMatrix.myMatrix[3][2];
		matrixProduct.myMatrix[2][3] = aMatrix.myMatrix[2][0] * anotherMatrix.myMatrix[0][3] + aMatrix.myMatrix[2][1] * anotherMatrix.myMatrix[1][3] + aMatrix.myMatrix[2][2] * anotherMatrix.myMatrix[2][3] + aMatrix.myMatrix[2][3] * anotherMatrix.myMatrix[3][3];					 
		matrixProduct.myMatrix[3][0] = aMatrix.myMatrix[3][0] * anotherMatrix.myMatrix[0][0] + aMatrix.myMatrix[3][1] * anotherMatrix.myMatrix[1][0] + aMatrix.myMatrix[3][2] * anotherMatrix.myMatrix[2][0] + aMatrix.myMatrix[3][3] * anotherMatrix.myMatrix[3][0];
		matrixProduct.myMatrix[3][1] = aMatrix.myMatrix[3][0] * anotherMatrix.myMatrix[0][1] + aMatrix.myMatrix[3][1] * anotherMatrix.myMatrix[1][1] + aMatrix.myMatrix[3][2] * anotherMatrix.myMatrix[2][1] + aMatrix.myMatrix[3][3] * anotherMatrix.myMatrix[3][1];
		matrixProduct.myMatrix[3][2] = aMatrix.myMatrix[3][0] * anotherMatrix.myMatrix[0][2] + aMatrix.myMatrix[3][1] * anotherMatrix.myMatrix[1][2] + aMatrix.myMatrix[3][2] * anotherMatrix.myMatrix[2][2] + aMatrix.myMatrix[3][3] * anotherMatrix.myMatrix[3][2];
		matrixProduct.myMatrix[3][3] = aMatrix.myMatrix[3][0] * anotherMatrix.myMatrix[0][3] + aMatrix.myMatrix[3][1] * anotherMatrix.myMatrix[1][3] + aMatrix.myMatrix[3][2] * anotherMatrix.myMatrix[2][3] + aMatrix.myMatrix[3][3] * anotherMatrix.myMatrix[3][3];

		return matrixProduct;
	}

	template <class T> Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
	{
		Vector4<T> product;
		product.x = aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1) + aVector.w * aMatrix(4, 1);
		product.y = aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2) + aVector.w * aMatrix(4, 2);
		product.z = aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3) + aVector.w * aMatrix(4, 3);
		product.w = aVector.x * aMatrix(1, 4) + aVector.y * aMatrix(2, 4) + aVector.z * aMatrix(3, 4) + aVector.w * aMatrix(4, 4);
		return product;

	}

	template <class T> Vector3<T> operator*(const Vector3<T>& aVector, const Matrix4x4<T>& aMatrix)
	{
		Vector3<T> product;
		product.x = aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1);
		product.y = aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2);
		product.z = aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3);
		return product;

	}

	template <class T> Vector4<T> operator*(const Matrix4x4<T>& aMatrix, const Vector4<T>& aVector)
	{
		Vector4<T> product;
		product.x = aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1) + aVector.w * aMatrix(4, 1);
		product.y = aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2) + aVector.w * aMatrix(4, 2);
		product.z = aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3) + aVector.w * aMatrix(4, 3);
		product.w = aVector.x * aMatrix(1, 4) + aVector.y * aMatrix(2, 4) + aVector.z * aMatrix(3, 4) + aVector.w * aMatrix(4, 4);
		return product;

	}


	template <class T> void operator*=(Matrix4x4<T>& aMatrix, const Matrix4x4<T>& anotherMatrix)
	{
		assert(aMatrix.myRows == anotherMatrix.myColumns);
		Matrix4x4<T> matrixProduct;

		matrixProduct.myMatrix[0][0] = aMatrix.myMatrix[0][0] * anotherMatrix.myMatrix[0][0] + aMatrix.myMatrix[0][1] * anotherMatrix.myMatrix[1][0] + aMatrix.myMatrix[0][2] * anotherMatrix.myMatrix[2][0] + aMatrix.myMatrix[0][3] * anotherMatrix.myMatrix[3][0];
		matrixProduct.myMatrix[0][1] = aMatrix.myMatrix[0][0] * anotherMatrix.myMatrix[0][1] + aMatrix.myMatrix[0][1] * anotherMatrix.myMatrix[1][1] + aMatrix.myMatrix[0][2] * anotherMatrix.myMatrix[2][1] + aMatrix.myMatrix[0][3] * anotherMatrix.myMatrix[3][1];
		matrixProduct.myMatrix[0][2] = aMatrix.myMatrix[0][0] * anotherMatrix.myMatrix[0][2] + aMatrix.myMatrix[0][1] * anotherMatrix.myMatrix[1][2] + aMatrix.myMatrix[0][2] * anotherMatrix.myMatrix[2][2] + aMatrix.myMatrix[0][3] * anotherMatrix.myMatrix[3][2];
		matrixProduct.myMatrix[0][3] = aMatrix.myMatrix[0][0] * anotherMatrix.myMatrix[0][3] + aMatrix.myMatrix[0][1] * anotherMatrix.myMatrix[1][3] + aMatrix.myMatrix[0][2] * anotherMatrix.myMatrix[2][3] + aMatrix.myMatrix[0][3] * anotherMatrix.myMatrix[3][3];
		matrixProduct.myMatrix[1][0] = aMatrix.myMatrix[1][0] * anotherMatrix.myMatrix[0][0] + aMatrix.myMatrix[1][1] * anotherMatrix.myMatrix[1][0] + aMatrix.myMatrix[1][2] * anotherMatrix.myMatrix[2][0] + aMatrix.myMatrix[1][3] * anotherMatrix.myMatrix[3][0];
		matrixProduct.myMatrix[1][1] = aMatrix.myMatrix[1][0] * anotherMatrix.myMatrix[0][1] + aMatrix.myMatrix[1][1] * anotherMatrix.myMatrix[1][1] + aMatrix.myMatrix[1][2] * anotherMatrix.myMatrix[2][1] + aMatrix.myMatrix[1][3] * anotherMatrix.myMatrix[3][1];
		matrixProduct.myMatrix[1][2] = aMatrix.myMatrix[1][0] * anotherMatrix.myMatrix[0][2] + aMatrix.myMatrix[1][1] * anotherMatrix.myMatrix[1][2] + aMatrix.myMatrix[1][2] * anotherMatrix.myMatrix[2][2] + aMatrix.myMatrix[1][3] * anotherMatrix.myMatrix[3][2];
		matrixProduct.myMatrix[1][3] = aMatrix.myMatrix[1][0] * anotherMatrix.myMatrix[0][3] + aMatrix.myMatrix[1][1] * anotherMatrix.myMatrix[1][3] + aMatrix.myMatrix[1][2] * anotherMatrix.myMatrix[2][3] + aMatrix.myMatrix[1][3] * anotherMatrix.myMatrix[3][3];
		matrixProduct.myMatrix[2][0] = aMatrix.myMatrix[2][0] * anotherMatrix.myMatrix[0][0] + aMatrix.myMatrix[2][1] * anotherMatrix.myMatrix[1][0] + aMatrix.myMatrix[2][2] * anotherMatrix.myMatrix[2][0] + aMatrix.myMatrix[2][3] * anotherMatrix.myMatrix[3][0];
		matrixProduct.myMatrix[2][1] = aMatrix.myMatrix[2][0] * anotherMatrix.myMatrix[0][1] + aMatrix.myMatrix[2][1] * anotherMatrix.myMatrix[1][1] + aMatrix.myMatrix[2][2] * anotherMatrix.myMatrix[2][1] + aMatrix.myMatrix[2][3] * anotherMatrix.myMatrix[3][1];
		matrixProduct.myMatrix[2][2] = aMatrix.myMatrix[2][0] * anotherMatrix.myMatrix[0][2] + aMatrix.myMatrix[2][1] * anotherMatrix.myMatrix[1][2] + aMatrix.myMatrix[2][2] * anotherMatrix.myMatrix[2][2] + aMatrix.myMatrix[2][3] * anotherMatrix.myMatrix[3][2];
		matrixProduct.myMatrix[2][3] = aMatrix.myMatrix[2][0] * anotherMatrix.myMatrix[0][3] + aMatrix.myMatrix[2][1] * anotherMatrix.myMatrix[1][3] + aMatrix.myMatrix[2][2] * anotherMatrix.myMatrix[2][3] + aMatrix.myMatrix[2][3] * anotherMatrix.myMatrix[3][3];
		matrixProduct.myMatrix[3][0] = aMatrix.myMatrix[3][0] * anotherMatrix.myMatrix[0][0] + aMatrix.myMatrix[3][1] * anotherMatrix.myMatrix[1][0] + aMatrix.myMatrix[3][2] * anotherMatrix.myMatrix[2][0] + aMatrix.myMatrix[3][3] * anotherMatrix.myMatrix[3][0];
		matrixProduct.myMatrix[3][1] = aMatrix.myMatrix[3][0] * anotherMatrix.myMatrix[0][1] + aMatrix.myMatrix[3][1] * anotherMatrix.myMatrix[1][1] + aMatrix.myMatrix[3][2] * anotherMatrix.myMatrix[2][1] + aMatrix.myMatrix[3][3] * anotherMatrix.myMatrix[3][1];
		matrixProduct.myMatrix[3][2] = aMatrix.myMatrix[3][0] * anotherMatrix.myMatrix[0][2] + aMatrix.myMatrix[3][1] * anotherMatrix.myMatrix[1][2] + aMatrix.myMatrix[3][2] * anotherMatrix.myMatrix[2][2] + aMatrix.myMatrix[3][3] * anotherMatrix.myMatrix[3][2];
		matrixProduct.myMatrix[3][3] = aMatrix.myMatrix[3][0] * anotherMatrix.myMatrix[0][3] + aMatrix.myMatrix[3][1] * anotherMatrix.myMatrix[1][3] + aMatrix.myMatrix[3][2] * anotherMatrix.myMatrix[2][3] + aMatrix.myMatrix[3][3] * anotherMatrix.myMatrix[3][3];
		aMatrix.myMatrix = matrixProduct.myMatrix;
	}

	template <class T> bool operator==(const Matrix4x4<T>& aMatrix, const Matrix4x4<T>& anotherMatrix)
	{
		for (size_t index = 0; index < aMatrix.myRows; ++index)
		{
			for (size_t index2 = 0; index2 < aMatrix.myColumns; ++index2)
			{
				if (aMatrix.myMatrix[index][index2] != anotherMatrix.myMatrix[index][index2])
				{
					return false;
				}
			}
		}
		return true;
	}
	typedef Matrix4x4<float> Matrix4x4f;
}
namespace CU = CommonUtilities;

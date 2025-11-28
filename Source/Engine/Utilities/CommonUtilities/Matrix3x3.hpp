#pragma once
#include "Vector3.hpp"
#include <array>
#include "Matrix.hpp"


namespace CommonUtilities
{
	template <class T>
	class Matrix4x4;

	template <class T>
		class Matrix3x3
		{
		public:
			// Creates the identity matrix.
			Matrix3x3<T>()
			{
				myMatrix[0][0] = static_cast<T> (1);
				myMatrix[0][1] = static_cast<T> (0);
				myMatrix[0][2] = static_cast<T> (0);
				myMatrix[1][0] = static_cast<T> (0);
				myMatrix[1][1] = static_cast<T> (1);
				myMatrix[1][2] = static_cast<T> (0);
				myMatrix[2][0] = static_cast<T> (0);
				myMatrix[2][1] = static_cast<T> (0);
				myMatrix[2][2] = static_cast<T> (1);
			}

			// Copy Constructor.
			Matrix3x3<T>(const Matrix3x3<T>& aMatrix)
			{
				for (int index = 0; index < myRows; ++index)
				{
					for (int index2 = 0; index2 < myColumns; ++index2)
					{
						myMatrix[index][index2] = aMatrix(index + 1, index2 + 1);
					}
				}
			}

			// Copies the top left 3x3 part of the Matrix4x4.
			Matrix3x3<T>(const Matrix4x4<T> & aMatrix)
			{
				for (int index = 0; index < myRows; ++index)
				{
					for (int index2 = 0; index2 < myColumns; ++index2)
					{
						myMatrix[index][index2] = aMatrix.myMatrix[index][index2];
					}
				}
			}

			// () operator for accessing element (row, column) for read/write or read, respectively.
			T& operator()(const int aRow, const int aColumn)
			{
				return myMatrix[aRow -1][aColumn - 1];
			}

			const T& operator()(const int aRow, const int aColumn) const
			{
				return myMatrix[aRow - 1][aColumn - 1];
			}

			Matrix3x3<T>& operator=(const Matrix3x3<T>& aMatrix) = default;

			// Static functions for creating rotation matrices.
			static Matrix3x3<T> CreateRotationAroundX(const T aAngleInRadians)
			{
				Matrix3x3<T> rotationMatrix;
				rotationMatrix(1, 1) = static_cast<T>(1);
				rotationMatrix(1, 2) = static_cast<T>(0);
				rotationMatrix(1, 3) = static_cast<T>(0);
					 
				rotationMatrix(2, 1) = static_cast<T>(0);
				rotationMatrix(2, 2) = static_cast<T>(cos(aAngleInRadians)); 
				rotationMatrix(2, 3) = static_cast<T>(sin(aAngleInRadians)); 
					 
				rotationMatrix(3, 1) = static_cast<T>(0);
				rotationMatrix(3, 2) = static_cast<T>(-1 * sin(aAngleInRadians)); 
				rotationMatrix(3, 3) = static_cast<T>(cos(aAngleInRadians)); 

				return rotationMatrix;

			}
			static Matrix3x3<T> CreateRotationAroundY(const T aAngleInRadians)
			{
				Matrix3x3<T> rotationMatrix;
				rotationMatrix(1, 1) = static_cast<T>(cos(aAngleInRadians));
				rotationMatrix(1, 2) = static_cast<T>(0);
				rotationMatrix(1, 3) = static_cast<T>(-1 * sin(aAngleInRadians));

				rotationMatrix(2, 1) = static_cast<T>(0);
				rotationMatrix(2, 2) = static_cast<T>(1);
				rotationMatrix(2, 3) = static_cast<T>(0);

				rotationMatrix(3, 1) = static_cast<T>(sin(aAngleInRadians));
				rotationMatrix(3, 2) = static_cast<T>(0);
				rotationMatrix(3, 3) = static_cast<T>(cos(aAngleInRadians));

				return rotationMatrix;
			}
			static Matrix3x3<T> CreateRotationAroundZ(const T aAngleInRadians)
			{
				Matrix3x3<T> rotationMatrix;
				rotationMatrix(1, 1) = static_cast<T>(cos(aAngleInRadians));
				rotationMatrix(1, 2) = static_cast<T>(sin(aAngleInRadians));
				rotationMatrix(1, 3) = static_cast<T>(0);

				rotationMatrix(2, 1) = static_cast<T>(-1 * sin(aAngleInRadians));
				rotationMatrix(2, 2) = static_cast<T>(cos(aAngleInRadians));
				rotationMatrix(2, 3) = static_cast<T>(0);

				rotationMatrix(3, 1) = static_cast<T>(0);
				rotationMatrix(3, 2) = static_cast<T>(0);
				rotationMatrix(3, 3) = static_cast<T>(1);


				return rotationMatrix;
			}

			// Static function for creating a transpose of a matrix.
			static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrixToTranspose)
			{
				static int statRows = 3;
				static int statColumns = 3;
				Matrix3x3<T> transposedMatrix;
				for (size_t index = 0; index < 3; ++index)
				{
					for (size_t index2 = 0; index2 < 3; ++index2)
					{
						transposedMatrix.myMatrix[index2][index] = aMatrixToTranspose.myMatrix[index][index2];
					}
				}
				return transposedMatrix;
			}

			size_t myRows = 3;
			size_t myColumns = 3;
			std::array<std::array<T, 3>, 3> myMatrix;

		};

		template <class T> Matrix3x3<T> operator+(const Matrix3x3<T> &aMatrix, const Matrix3x3<T> &anotherMatrix)
		{
			assert(aMatrix.myRows == anotherMatrix.myRows && aMatrix.myColumns == anotherMatrix.myColumns);
			Matrix3x3<T> aMatrixSum;
			for (size_t index = 0; index < aMatrix.myRows; ++index)
			{
				for (size_t index2 = 0; index2 < aMatrix.myColumns; ++index2)
				{
					aMatrixSum.myMatrix[index][index2] = aMatrix.myMatrix[index][index2] + anotherMatrix.myMatrix[index][index2];
				}
			}
			return aMatrixSum;

		}

		template <class T> void operator+=(Matrix3x3<T> &aMatrix, const Matrix3x3<T> &anotherMatrix)
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

		template <class T> Matrix3x3<T> operator-(const Matrix3x3<T> &aMatrix, const Matrix3x3<T> &anotherMatrix)
		{
			assert(aMatrix.myRows == anotherMatrix.myRows && aMatrix.myColumns == anotherMatrix.myColumns);

			Matrix3x3<T> aMatrixSum;
			for (size_t index = 0; index < aMatrix.myRows; ++index)
			{
				for (size_t index2 = 0; index2 < aMatrix.myColumns; ++index2)
				{
					aMatrixSum.myMatrix[index][index2] = aMatrix.myMatrix[index][index2] - anotherMatrix.myMatrix[index][index2];
				}
			}
			return aMatrixSum;
		}

		template <class T> void operator-=(Matrix3x3<T> &aMatrix, const Matrix3x3<T> &anotherMatrix)
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


		template <class T> Matrix3x3<T> operator*(const Matrix3x3<T>& aMatrix, const Matrix3x3<T>& anotherMatrix)
		{
			assert(aMatrix.myRows == anotherMatrix.myColumns);
			Matrix3x3<T> matrixProduct;

			matrixProduct.myMatrix[0][0] = aMatrix.myMatrix[0][0] * anotherMatrix.myMatrix[0][0] + aMatrix.myMatrix[0][1] * anotherMatrix.myMatrix[1][0] + aMatrix.myMatrix[0][2] * anotherMatrix.myMatrix[2][0];
			matrixProduct.myMatrix[0][1] = aMatrix.myMatrix[0][0] * anotherMatrix.myMatrix[0][1] + aMatrix.myMatrix[0][1] * anotherMatrix.myMatrix[1][1] + aMatrix.myMatrix[0][2] * anotherMatrix.myMatrix[2][1];
			matrixProduct.myMatrix[0][2] = aMatrix.myMatrix[0][0] * anotherMatrix.myMatrix[0][2] + aMatrix.myMatrix[0][1] * anotherMatrix.myMatrix[1][2] + aMatrix.myMatrix[0][2] * anotherMatrix.myMatrix[2][2];
			matrixProduct.myMatrix[1][0] = aMatrix.myMatrix[1][0] * anotherMatrix.myMatrix[0][0] + aMatrix.myMatrix[1][1] * anotherMatrix.myMatrix[1][0] + aMatrix.myMatrix[1][2] * anotherMatrix.myMatrix[2][0];
			matrixProduct.myMatrix[1][1] = aMatrix.myMatrix[1][0] * anotherMatrix.myMatrix[0][1] + aMatrix.myMatrix[1][1] * anotherMatrix.myMatrix[1][1] + aMatrix.myMatrix[1][2] * anotherMatrix.myMatrix[2][1];
			matrixProduct.myMatrix[1][2] = aMatrix.myMatrix[1][0] * anotherMatrix.myMatrix[0][2] + aMatrix.myMatrix[1][1] * anotherMatrix.myMatrix[1][2] + aMatrix.myMatrix[1][2] * anotherMatrix.myMatrix[2][2];
			matrixProduct.myMatrix[2][0] = aMatrix.myMatrix[2][0] * anotherMatrix.myMatrix[0][0] + aMatrix.myMatrix[2][1] * anotherMatrix.myMatrix[1][0] + aMatrix.myMatrix[2][2] * anotherMatrix.myMatrix[2][0];
			matrixProduct.myMatrix[2][1] = aMatrix.myMatrix[2][0] * anotherMatrix.myMatrix[0][1] + aMatrix.myMatrix[2][1] * anotherMatrix.myMatrix[1][1] + aMatrix.myMatrix[2][2] * anotherMatrix.myMatrix[2][1];
			matrixProduct.myMatrix[2][2] = aMatrix.myMatrix[2][0] * anotherMatrix.myMatrix[0][2] + aMatrix.myMatrix[2][1] * anotherMatrix.myMatrix[1][2] + aMatrix.myMatrix[2][2] * anotherMatrix.myMatrix[2][2];
			
			return matrixProduct;
		}

		template <class T> Vector3<T> operator*(const Vector3<T> & aVector,const Matrix3x3<T>& aMatrix)
		{
			Vector3<T> product;
			product.x = aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1);
			product.y = aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2);
			product.z = aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3);
			return product;

		}

		template <class T> void operator*=(Matrix3x3<T>& aMatrix, const Matrix3x3<T>& anotherMatrix)
		{
			assert(aMatrix.myRows == anotherMatrix.myColumns);
			Matrix3x3<T> matrixProduct;

			matrixProduct.myMatrix[0][0] = aMatrix.myMatrix[0][0] * anotherMatrix.myMatrix[0][0] + aMatrix.myMatrix[0][1] * anotherMatrix.myMatrix[1][0] + aMatrix.myMatrix[0][2] * anotherMatrix.myMatrix[2][0];
			matrixProduct.myMatrix[0][1] = aMatrix.myMatrix[0][0] * anotherMatrix.myMatrix[0][1] + aMatrix.myMatrix[0][1] * anotherMatrix.myMatrix[1][1] + aMatrix.myMatrix[0][2] * anotherMatrix.myMatrix[2][1];
			matrixProduct.myMatrix[0][2] = aMatrix.myMatrix[0][0] * anotherMatrix.myMatrix[0][2] + aMatrix.myMatrix[0][1] * anotherMatrix.myMatrix[1][2] + aMatrix.myMatrix[0][2] * anotherMatrix.myMatrix[2][2];
			matrixProduct.myMatrix[1][0] = aMatrix.myMatrix[1][0] * anotherMatrix.myMatrix[0][0] + aMatrix.myMatrix[1][1] * anotherMatrix.myMatrix[1][0] + aMatrix.myMatrix[1][2] * anotherMatrix.myMatrix[2][0];
			matrixProduct.myMatrix[1][1] = aMatrix.myMatrix[1][0] * anotherMatrix.myMatrix[0][1] + aMatrix.myMatrix[1][1] * anotherMatrix.myMatrix[1][1] + aMatrix.myMatrix[1][2] * anotherMatrix.myMatrix[2][1];
			matrixProduct.myMatrix[1][2] = aMatrix.myMatrix[1][0] * anotherMatrix.myMatrix[0][2] + aMatrix.myMatrix[1][1] * anotherMatrix.myMatrix[1][2] + aMatrix.myMatrix[1][2] * anotherMatrix.myMatrix[2][2];
			matrixProduct.myMatrix[2][0] = aMatrix.myMatrix[2][0] * anotherMatrix.myMatrix[0][0] + aMatrix.myMatrix[2][1] * anotherMatrix.myMatrix[1][0] + aMatrix.myMatrix[2][2] * anotherMatrix.myMatrix[2][0];
			matrixProduct.myMatrix[2][1] = aMatrix.myMatrix[2][0] * anotherMatrix.myMatrix[0][1] + aMatrix.myMatrix[2][1] * anotherMatrix.myMatrix[1][1] + aMatrix.myMatrix[2][2] * anotherMatrix.myMatrix[2][1];
			matrixProduct.myMatrix[2][2] = aMatrix.myMatrix[2][0] * anotherMatrix.myMatrix[0][2] + aMatrix.myMatrix[2][1] * anotherMatrix.myMatrix[1][2] + aMatrix.myMatrix[2][2] * anotherMatrix.myMatrix[2][2];
			aMatrix.myMatrix = matrixProduct.myMatrix;
		}

		template <class T> bool operator==(const Matrix3x3<T>& aMatrix,const Matrix3x3<T>& anotherMatrix)
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
		typedef Matrix3x3<float> Matrix3x3f;
}
namespace CU = CommonUtilities;


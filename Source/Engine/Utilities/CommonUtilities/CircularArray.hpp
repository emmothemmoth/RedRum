#pragma once

#include <cassert>
#include <array>

namespace CommonUtilities
{
	template<typename Type, int size>
	class CircularArray
	{
	public:
		CircularArray();
		CircularArray(const CircularArray& anoterhArray);
		~CircularArray();

		void Emplace(const Type& anElement);

		CircularArray& operator=(const CircularArray& anotherArray)
		{
			for (int index = 0; index < size; index++)
			{
				myArray[index] = anotherArray[index];
			}
		}

		inline const Type& operator[] (const size_t anIndex) const
		{
			assert(anIndex < myCapacity);
			assert(anIndex >= 0);
			return myArray[anIndex % myCapacity];
		}

		size_t Size() const { return mySize; }

		size_t Capacity() const { return myCapacity; }

	private:
		std::array<Type, size> myArray;
		size_t mySize = 0;
		int myCurrentIndex = 0;
		size_t myCapacity = size;

	};
	template<typename Type, int size>
	inline CircularArray<Type, size>::CircularArray()
	{
		myCapacity = size;
		mySize = 0;
		myCurrentIndex = 0;
	}
	template<typename Type, int size>
	inline CircularArray<Type, size>::CircularArray(const CircularArray& anotherArray)
	{
		myCapacity = anotherArray.Capacity();
		mySize = anotherArray.Size();
		for (int index = 0; index < anotherArray.Size(); index++)
		{
			myArray[index] = anotherArray[index];
		}
	}
	template<typename Type, int size>
	inline CircularArray<Type, size>::~CircularArray()
	{
		myCapacity = 0;
		myCurrentIndex = 0;
		mySize = 0;
	}
	template<typename Type, int size>
	inline void CircularArray<Type, size>::Emplace(const Type& anElement)
	{
		myArray[myCurrentIndex % myCapacity] = anElement;
		myCurrentIndex = (myCurrentIndex + 1) % myCapacity;
		if (mySize < myCapacity)
		{
			mySize++;
		}
	}
}

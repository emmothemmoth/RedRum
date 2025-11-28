#pragma once
#include <array>
#include <assert.h>

namespace CommonUtilities
{
	template<typename Type, int size, typename CountType=unsigned short, bool UseSafeModeFlag=true>
	class VectorOnStack
	{
	public:
		VectorOnStack();
		VectorOnStack(const VectorOnStack& aVectorOnStack);
		~VectorOnStack();

		VectorOnStack& operator=(const VectorOnStack& aVectorOnStack);
		inline const Type& operator[] (const CountType anIndex) const;
		inline Type& operator[] (const CountType anIndex);

		inline void Add(const Type& anObject);
		inline void Insert(const CountType anIndex, const Type& anObject);
		inline void RemoveCyclic(const Type& anObject);
		inline void RemoveCyclicAtIndex(const CountType anIndex);

		inline void Clear();
		inline CountType Size() const;

	private:
		std::array<Type, size> myArray;
		int myCurrentSize;
		int myMaxSize;
	};

	template <typename Type, int size, typename CountType, bool UseSafeModeFlag>
	VectorOnStack<Type, size, CountType, UseSafeModeFlag>::VectorOnStack()
	{
		myMaxSize = size;
		myCurrentSize = 0;
	}

	template <typename Type, int size, typename CountType, bool UseSafeModeFlag>
	VectorOnStack<Type, size, CountType, UseSafeModeFlag>::VectorOnStack(const VectorOnStack& aVectorOnStack)
	{
		myMaxSize = size;
		myCurrentSize = aVectorOnStack.Size();
		for (unsigned short index = 0; index < aVectorOnStack.Size(); index++)
		{
			myArray[index] = aVectorOnStack[index];
		}
		assert(myCurrentSize > 0);
	}

	template <typename Type, int size, typename CountType, bool UseSafeModeFlag>
	VectorOnStack<Type, size, CountType, UseSafeModeFlag>::~VectorOnStack()
	{
		myCurrentSize = 0;
		myMaxSize = 0;
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	VectorOnStack<Type, size, CountType, UseSafeModeFlag>& VectorOnStack<Type, size, CountType, UseSafeModeFlag>::operator=(const VectorOnStack<Type, size, CountType, UseSafeModeFlag>& aVectorOnStack)
	{
		myCurrentSize = aVectorOnStack.Size();
		for (int index = 0; index < myCurrentSize; index++)
		{
			myArray[index] = aVectorOnStack[index];
		}
		return *this;
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline const Type& VectorOnStack<Type, size, CountType, UseSafeModeFlag>::operator[](const CountType anIndex) const
	{
		assert(anIndex < myCurrentSize);
		assert(anIndex >= 0);

		return myArray[anIndex];
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline Type& VectorOnStack<Type, size, CountType, UseSafeModeFlag>::operator[](const CountType anIndex)
	{
		assert(anIndex < myCurrentSize);
		assert(anIndex >= 0);

		return myArray[anIndex];
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, size, CountType, UseSafeModeFlag>::Add(const Type& anObject)
	{
		assert(myCurrentSize + 1 <= myMaxSize);
	
		myArray[myCurrentSize] = anObject;
		myCurrentSize++;
		
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, size, CountType, UseSafeModeFlag>::Insert(const CountType anIndex, const Type& anObject)
	{
		assert(myCurrentSize < myMaxSize);
		assert(anIndex >= 0);
		assert(anIndex < myMaxSize);
		
		for (int index = myCurrentSize; index > anIndex; index--)
		{
			
			myArray[index] = myArray[index - 1];
			
			
		}
		myArray[anIndex] = anObject;
		myCurrentSize++;
	}



	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, size, CountType, UseSafeModeFlag>::RemoveCyclic(const Type& anObject)
	{
		for (int index = 0; index < myCurrentSize; index++)
		{
			if (myArray[index] == anObject)
			{
				myArray[index] = myArray[myCurrentSize-1];
				myCurrentSize--;
				index = myCurrentSize;
			}
		}
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, size, CountType, UseSafeModeFlag>::RemoveCyclicAtIndex(const CountType anIndex)
	{
		assert(anIndex < myCurrentSize);
		assert(anIndex >= 0);
		myArray[anIndex] = myArray[myCurrentSize-1];
		myCurrentSize--;
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, size, CountType, UseSafeModeFlag>::Clear()
	{
		myCurrentSize = 0;
	}

	template<typename Type, int size, typename CountType, bool UseSafeModeFlag>
	__forceinline CountType VectorOnStack<Type, size, CountType, UseSafeModeFlag>::Size() const
	{
		return myCurrentSize;
	}
}

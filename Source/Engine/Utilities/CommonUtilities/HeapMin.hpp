#pragma once

#pragma once
#include <vector>

namespace CommonUtilities
{
	template <class T>
	class HeapMin
	{
	public:
		HeapMin();
		~HeapMin();

		//Returnerar antal element i heapen
		int GetSize() const;
		//Lägger till elementet i heapen
		void Enqueue(const T& aElement); //lägger till elementet i heapen
		//Returnerar det största elementet i heapen
		const T& GetTop() const;
		//Tar bort det största elementet ur heapen och returnerar det
		T Dequeue();

	private:
		void BubbleUp(const size_t& anIndex);
		void BubbleDown(const size_t& anIndex);

		std::vector<T> myHeap;
	};
	template<class T>
	inline HeapMin<T>::HeapMin()
	{
	}
	template<class T>
	inline HeapMin<T>::~HeapMin()
	{
		myHeap.clear();
	}
	template<class T>
	inline int HeapMin<T>::GetSize() const
	{
		return static_cast<int>(myHeap.size());
	}
	template<class T>
	inline void HeapMin<T>::Enqueue(const T& aElement)
	{
		myHeap.push_back(aElement);
		if (1 < myHeap.size())
		{
			BubbleUp(myHeap.size() - 1);
		}
	}
	template<class T>
	inline const T& HeapMin<T>::GetTop() const
	{
		if (0 < myHeap.size())
		{
			return myHeap[0];
		}
		T zero = 0;
		return zero;
	}
	template<class T>
	inline T HeapMin<T>::Dequeue()
	{
		T top = 0;
		if (0 < myHeap.size())
		{
			top = myHeap[0];
			myHeap[0] = myHeap[myHeap.size() - 1];
			myHeap.erase(myHeap.begin() + (myHeap.size() - 1));
			if (1 < myHeap.size())
			{
				BubbleDown(0);
			}

		}
		return top;
	}
	template<class T>
	inline void HeapMin<T>::BubbleUp(const size_t& anIndex)
	{
		size_t parentIndex = (anIndex - 1) / 2;
		if (myHeap[anIndex] < myHeap[parentIndex])
		{
			T temp = myHeap[parentIndex];
			myHeap[parentIndex] = myHeap[anIndex];
			myHeap[anIndex] = temp;
			if (0 < parentIndex)
			{
				BubbleUp(parentIndex);
			}
		}
	}
	template<class T>
	inline void HeapMin<T>::BubbleDown(const size_t& anIndex)
	{
		size_t childIndexLeft = 2 * anIndex + 1;
		size_t childIndexRight = 2 * anIndex + 2;

		if ((myHeap.size() - 1) < childIndexLeft || (myHeap.size() - 1) < childIndexRight)
		{
			return;
		}
		if (myHeap[childIndexLeft] < myHeap[childIndexRight])
		{
			//if left child is smaller - > check against left
			if (myHeap[childIndexLeft] < myHeap[anIndex])
			{
				T temp = myHeap[childIndexLeft];
				myHeap[childIndexLeft] = myHeap[anIndex];
				myHeap[anIndex] = temp;
				BubbleDown(childIndexLeft);
				return;
			}
		}
		if (myHeap[childIndexRight] < myHeap[anIndex])
		{
			T temp = myHeap[childIndexRight];
			myHeap[childIndexRight] = myHeap[anIndex];
			myHeap[anIndex] = temp;
			BubbleDown(childIndexRight);
			return;
		}
	}
}


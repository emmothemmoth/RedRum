#pragma once
#include <vector>

namespace CommonUtilities
{
	template <class T>
	class Heap
	{
	public:
		Heap();
		~Heap();

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
	inline Heap<T>::Heap()
	{
	}
	template<class T>
	inline Heap<T>::~Heap()
	{
		myHeap.clear();
	}
	template<class T>
	inline int Heap<T>::GetSize() const
	{
		return static_cast<int>(myHeap.size());
	}
	template<class T>
	inline void Heap<T>::Enqueue(const T& aElement)
	{
		myHeap.push_back(aElement);
		if (1 < myHeap.size())
		{
			BubbleUp(myHeap.size() - 1);
		}
	}
	template<class T>
	inline const T& Heap<T>::GetTop() const
	{
		if (0 < myHeap.size())
		{
			return myHeap[0];
		}
		T zero = 0;
		return zero;
	}
	template<class T>
	inline T Heap<T>::Dequeue()
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
	inline void Heap<T>::BubbleUp(const size_t& anIndex)
	{
		size_t parentIndex = (anIndex - 1) / 2;
		if (myHeap[parentIndex] < myHeap[anIndex])
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
	inline void Heap<T>::BubbleDown(const size_t& anIndex)
	{
		size_t childIndexLeft = 2 * anIndex + 1;
		size_t childIndexRight = 2 * anIndex + 2;

		if ((myHeap.size() - 1) < childIndexLeft || (myHeap.size() - 1) < childIndexRight)
		{
			return;
		}
		if (myHeap[childIndexRight] < myHeap[childIndexLeft])
		{
			//if right child is smaller - > check against left
			if (myHeap[anIndex] < myHeap[childIndexLeft])
			{
				T temp = myHeap[childIndexLeft];
				myHeap[childIndexLeft] = myHeap[anIndex];
				myHeap[anIndex] = temp;
				BubbleDown(childIndexLeft);
				return;
			}
		}
		if (myHeap[anIndex] < myHeap[childIndexRight])
		{
			T temp = myHeap[childIndexRight];
			myHeap[childIndexRight] = myHeap[anIndex];
			myHeap[anIndex] = temp;
			BubbleDown(childIndexRight);
			return;
		}
	}
}

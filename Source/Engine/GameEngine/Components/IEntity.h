#pragma once
#include "ComponentID.h"
#include "CommonUtilities\Matrix4x4.hpp"

#include <vector>
#include <memory>

class Component;


class IEntity
{
public:
	IEntity();
	virtual ~IEntity();
	virtual void Update(const float& aDeltaTime);
	virtual void Render();
	void AddComponent(std::shared_ptr<Component> aComponent);

	template <class T>
	std::shared_ptr<T> GetComponent();

	template <class T>
	std::vector<std::shared_ptr<T>> GetComponents();

	template <class T>
	bool RemoveComponent();

	CommonUtilities::Matrix4x4<float> GetTransform();

	CommonUtilities::Vector3<float> GetPosition();

	void SetTransform(CommonUtilities::Matrix4x4<float>& aTransform);
				 

protected:
	CommonUtilities::Matrix4x4<float> myTransform;
	std::vector<std::shared_ptr<Component>>	myComponents;

private:
	//std::vector<std::shared_ptr<Component>>	myComponents;
};


template<class T>
inline std::shared_ptr<T> IEntity::GetComponent()
{
	for (auto& c : myComponents)
	{
		std::shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(c);

		if (ptr.get() != nullptr)
		{
			return ptr;
		}

	}
	return std::shared_ptr<T>();
}

template<class T>
inline std::vector<std::shared_ptr<T>> IEntity::GetComponents()
{
	std::vector<T> componentsVector;

	for (auto& c : myComponents)
	{
		std::shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(c);

		if (ptr.get() != nullptr)
		{
			componentsVector.push_back(ptr);
		}
	}
	return componentsVector;
}

template<class T>
inline bool IEntity::RemoveComponent()
{
	for (size_t index = 0; index < myComponents.size(); index++)
	{
		std::shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(myComponents[index]);

		if (ptr.get() != nullptr)
		{
			myComponents.erase(myComponents.begin() + index);
			return true;
		}
	}
	return false;
}


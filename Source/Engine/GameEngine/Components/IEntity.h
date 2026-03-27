#pragma once

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

	template<class T>
	std::shared_ptr<T> GetLastAddedComponent();

	template <class T>
	std::vector<std::shared_ptr<T>> GetComponents();

	template <class T>
	bool RemoveComponent();

	void SetIsActive(const bool aIsActive) { myIsActive = aIsActive; }
	bool GetIsActive() const { return myIsActive; }

	void SetVisible(const bool aIsVisible) { myIsVisible = aIsVisible; }
	bool GetIsVisible() const { return myIsVisible; }

protected:
	std::vector<std::shared_ptr<Component>>	myComponents;
	bool myIsActive = true;
	bool myIsVisible = true;
	unsigned myComponentIDCounter = 0;
private:
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
inline std::shared_ptr<T> IEntity::GetLastAddedComponent()
{
	assert(myComponents.size() > 0);
	std::shared_ptr<T> cPtr = nullptr;
	for (auto& c : myComponents)
	{
		std::shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(c);
		if (ptr.get() != nullptr)
		{
			cPtr = ptr;
		}
	}
	return cPtr;
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


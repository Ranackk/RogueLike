#pragma once
#include "Transform.h"
#include "Component.h"
#include "UpdateComponent.h"

class Material;
class CameraComponent;

class GameObject
{
public:
	GameObject();
	explicit GameObject(std::string _name);
	virtual ~GameObject() {}

	virtual void update(GLFWwindow* window, const float deltaTime);

	Transform& getTransform() { return m_Transform; }
	std::string getName() const { return m_Name; }

	void setActive(const bool _active) { m_Active = _active; }
	bool isActive() const { return m_Active; }

	template <class T>
	T* addComponent(T* component);
	template <class T>
	T* getComponent() const;


protected:
	Transform m_Transform;
	std::string m_Name;
	bool m_Active = true;

	std::vector<class Component*> m_Components;

	// READONLY!
	std::vector<class UpdateComponent*> m_UpdateComponents;
};

template <typename T>
T* GameObject::addComponent(T* component) {
	m_Components.push_back(component);
	(component)->setGameObject(this);

	// Update Vector of all update components
	m_UpdateComponents = std::vector<UpdateComponent*>();
	for (auto i = m_Components.begin(); i < m_Components.end(); ++i) {
		UpdateComponent* comp = dynamic_cast<UpdateComponent*> (*i);
		if (comp != nullptr) {
			m_UpdateComponents.push_back(comp);
		}
	}

	return component;
}

template <typename T>
T* GameObject::getComponent() const {
	for (auto i = m_Components.begin(); i < m_Components.end(); ++i) {
		T* comp = dynamic_cast<T*> (*i);
		if (comp != nullptr) return comp;
	}
	return nullptr;
}


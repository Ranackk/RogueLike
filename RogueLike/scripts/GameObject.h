#pragma once
#include "Transform.h"
#include "Component.h"

class Material;
class Camera;

class GameObject
{
public:
	GameObject();
	virtual ~GameObject() {}

	virtual void update(GLFWwindow* window, const float deltaTime) {};

	Transform& getTransform() { return m_Transform; }

	template <class T>
	T* addComponent(T* component);
	template <class T>
	T* getComponent() const;

protected:
	Transform m_Transform;

	std::vector<class Component*> m_Components;
};

template <typename T>
T* GameObject::addComponent(T* component) {
	m_Components.push_back(component);
	(component)->setGameObject(this);
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


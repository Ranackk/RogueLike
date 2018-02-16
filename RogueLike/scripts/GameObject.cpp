#include "stdafx.h"
#include "GameObject.h"

GameObject::GameObject() {
	m_Transform = Transform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
}

void GameObject::update(GLFWwindow* window, const float deltaTime) {
	for (auto i = m_UpdateComponents.begin(); i < m_UpdateComponents.end(); ++i) {
		(*i)->update(window, deltaTime);
	}
}

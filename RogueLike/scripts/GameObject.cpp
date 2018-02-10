#include "stdafx.h"
#include "GameObject.h"

GameObject::GameObject() {
	m_Transform = Transform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
}
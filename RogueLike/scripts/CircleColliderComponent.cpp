#include "stdafx.h"
#include "CircleColliderComponent.h"

CircleColliderComponent::CircleColliderComponent()
{
	m_CircleCollider = CircleCollider(0, glm::vec3());
}

CircleCollider& CircleColliderComponent::getCollider() {
	return m_CircleCollider;
}

void CircleColliderComponent::initialize(const CircleCollider &_collider) {
	m_CircleCollider = _collider;
}

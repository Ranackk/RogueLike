#include "stdafx.h"
#include "BoxColliderComponent.h"


BoxColliderComponent::BoxColliderComponent() {
	m_BoxCollider = BoxCollider(glm::vec2(), glm::vec3());
}
BoxCollider& BoxColliderComponent::getCollider() {
	return m_BoxCollider;
}

void BoxColliderComponent::initialize(const BoxCollider& _collider) {
	m_BoxCollider = _collider;
}

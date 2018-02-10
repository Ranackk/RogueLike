#include "stdafx.h"
#include "BoxCollider.h"
#include "PhysicsEngine.h"

BoxCollider::BoxCollider(glm::vec2 _dimension, const glm::vec3 _midpointOffset) {
	this->m_Dimensions = _dimension;
	this->m_MidpointOffset = _midpointOffset;
}

bool BoxCollider::collidesWith(Collider& other) {
	return other.collidesWithBox(*this);
}
bool BoxCollider::colldesWithCircle(CircleCollider& circleCollider) { return PhysicsEngine::collides(*this, circleCollider); }
bool BoxCollider::collidesWithBox(BoxCollider& boxCollider) { return PhysicsEngine::collides(*this, boxCollider); }

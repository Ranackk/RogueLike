#include "stdafx.h"
#include "CircleCollider.h"
#include "PhysicsEngine.h"

CircleCollider::CircleCollider(const float _radius, const glm::vec3 _midpointOffset) {
	this->m_Radius = _radius;
	this->m_MidpointOffset = _midpointOffset;
}

bool CircleCollider::collidesWith(Collider& other) { 
	return other.colldesWithCircle(*this);
}
bool CircleCollider::colldesWithCircle(CircleCollider& circleCollider) { return PhysicsEngine::collides(*this, circleCollider); }
bool CircleCollider::collidesWithBox(BoxCollider& boxCollider) { return PhysicsEngine::collides(boxCollider, *this); }

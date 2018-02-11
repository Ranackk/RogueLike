#include "stdafx.h"
#include "Collider.h"
#include "GameObject.h"

void Collider::initialize(const std::shared_ptr<GameObject> _gameObject) {
	this->m_GameObject = _gameObject;
	this->m_CollisionLayer = 0;
}

Collider::~Collider() {
	
}

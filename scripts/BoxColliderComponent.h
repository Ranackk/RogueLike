#pragma once
#include "BoxCollider.h"
#include "ColliderComponent.h"

class BoxColliderComponent : public ColliderComponent
{
public:
	BoxColliderComponent();
	void initialize(const BoxCollider &_collider);
	BoxCollider& getCollider();

private:
	BoxCollider m_BoxCollider;
};


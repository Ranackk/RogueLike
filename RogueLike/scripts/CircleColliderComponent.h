#pragma once
#include "CircleCollider.h"

class CircleColliderComponent : public ColliderComponent
{
public:
	CircleColliderComponent();

	void initialize(const CircleCollider &_collider);
	CircleCollider& getCollider();

private:
	CircleCollider m_CircleCollider;
};


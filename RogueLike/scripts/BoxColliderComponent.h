#pragma once
#include "BoxCollider.h"

class BoxColliderComponent : public Component
{
public:
	BoxColliderComponent();
	void initialize(const BoxCollider &_collider);
	BoxCollider& getCollider();

private:
	BoxCollider m_BoxCollider;
};


#pragma once
#include "ColliderComponent.h"
#include "Collider.h"

/**
 * \brief Class that describes a Circular ColliderComponent */
class CircleCollider : public Collider
{
public:
	CircleCollider() {};
	explicit CircleCollider(const float _radius, const glm::vec3 _midpointOffset);

	float m_Radius = 0;
	glm::vec3 m_MidpointOffset;

	bool collidesWith(Collider& other) override;
	bool colldesWithCircle(CircleCollider& circleCollider) override;
	bool collidesWithBox(BoxCollider& boxCollider) override;
};


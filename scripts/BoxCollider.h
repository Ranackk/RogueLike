#pragma once
#include "Collider.h"

/**
 * \brief Class that describes an Axis Aligned Rectangular ColliderComponent */
class BoxCollider : public Collider
{
public:
	BoxCollider() {};
	explicit BoxCollider(glm::vec2 _dimension, const glm::vec3 _midpointOffset);

	glm::vec2 m_Dimensions;
	glm::vec3 m_MidpointOffset;

	bool collidesWith(Collider& other) override;
	bool colldesWithCircle(CircleCollider& circleCollider) override;
	bool collidesWithBox(BoxCollider& boxCollider) override;
};


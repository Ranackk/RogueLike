#pragma once
#include "GameObject.h"
#include "EnemyComponent.h"
#include "CircleColliderComponent.h"

class ProjectileComponent : public UpdateComponent
{
public:
	ProjectileComponent();

	float m_MovementSpeed;
	glm::vec3 m_Direction;
	CircleCollider* m_CircleCollider;
	CircleColliderComponent* m_CircleColliderComponent;

	void update(GLFWwindow* window, const float deltaTime) override;
	void die() const;
	void initialize(const glm::vec3 _position, const glm::vec3 _direction, const float _speed, const unsigned char _layer);
};

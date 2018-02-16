#pragma once
#include "GameObject.h"
#include "Enemy.h"
#include "CircleColliderComponent.h"

class Projectile : public GameObject
{
public:
	Projectile();

	Scene* m_Scene;
	float m_MovementSpeed;
	glm::vec3 m_Direction;
	CircleCollider* m_CircleCollider;
	CircleColliderComponent* m_CircleColliderComponent;

	void update(GLFWwindow* window, const float deltaTime) override;
	void initialize(Scene* _scene, const glm::vec3 _position, const glm::vec3 _direction, const float _speed, const unsigned char _layer);
};


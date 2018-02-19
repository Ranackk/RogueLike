#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "BoxCollider.h"
#include "UpdateComponent.h"
#include "HealthComponent.h"

class Scene;

/**
* \brief Represents a EnemyComponent that has 1hp and deals 1 damage on player collision
*/
class EnemyComponent :
	public UpdateComponent
{
public:
	EnemyComponent();
	void initialize(Scene* _scene, glm::vec3 _position);

	void update(GLFWwindow* window, const float deltaTime) override;

	void takeDamage(const float _amount);
	void die();
private:

	Scene* m_Scene;
	float m_MovementSpeed;
	CircleCollider* m_CircleCollider;
	HealthComponent* m_HealthComponent;

	// DEMO AI (Replace Later)
	glm::vec3 m_StartPosition;
	float m_AliveTime = 0.0f;
};
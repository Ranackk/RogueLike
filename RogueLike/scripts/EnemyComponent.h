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
	enum EnemyType {
		BASE
	};

	EnemyComponent();
	void initialize(Scene* _scene, const EnemyType _type, glm::vec3 _position);

	void update(GLFWwindow* window, const float deltaTime) override;

	void takeDamage(const float _amount);
	void die();

	EnemyType getType() const { return m_Type; }
private:

	EnemyType m_Type;
	Scene* m_Scene;
	float m_MovementSpeed;
	CircleCollider* m_CircleCollider;
	HealthComponent* m_HealthComponent;

	// DEMO AI (Replace Later)
	glm::vec3 m_StartPosition;
	float m_AliveTime = 0.0f;
};
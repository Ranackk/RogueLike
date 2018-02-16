#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "BoxCollider.h"
#include "UpdateComponent.h"

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
	void die();
	bool isAlive() const;
	~EnemyComponent();
private:
	bool m_Alive = true;

	Scene* m_Scene;
	float m_MovementSpeed;
	CircleCollider* m_CircleCollider;
};
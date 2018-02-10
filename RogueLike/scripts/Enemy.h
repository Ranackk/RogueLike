#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "BoxCollider.h"

class Scene;

/**
* \brief Represents a Enemy that has 1hp and deals 1 damage on player collision
*/
class Enemy :
	public GameObject
{
public:
	Enemy();
	void initialize(Scene* _scene, glm::vec3 _position);

	void update(GLFWwindow* window, const float deltaTime) override;
	void die();
	bool isAlive() const;
	~Enemy();
private:
	bool m_Alive = true;

	Scene* m_Scene;
	float m_MovementSpeed;
	CircleCollider* m_CircleCollider;
};
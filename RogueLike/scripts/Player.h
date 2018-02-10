#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "Light.h"

class Player : public GameObject
{
public:
	Player();
	void initialize();
	class Light* m_Light;
	float m_MovementSpeed;

	glm::vec3 m_LightOffset;

	void update(GLFWwindow* window, const float deltaTime) override;

private:

	glm::vec3 m_LastPosition;
};


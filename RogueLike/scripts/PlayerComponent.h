#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "LightComponent.h"

class PlayerComponent : public UpdateComponent
{
public:
	PlayerComponent();
	void initialize();
	class LightComponent* m_Light;
	float m_MovementSpeed;

	glm::vec3 m_LightOffset;

	void update(GLFWwindow* window, const float deltaTime) override;

private:

	glm::vec3 m_LastPosition;

	glm::vec3 m_FacingDirection;
};


#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "LightComponent.h"

class PlayerComponent : public UpdateComponent
{
public:
	PlayerComponent();
	void initialize();
	void takeDamage(const float _amount);
	class LightComponent* m_Light;
	float m_MovementSpeed;

	glm::vec3 m_LightOffset;

	void update(GLFWwindow* window, const float deltaTime) override;

private:
	std::shared_ptr<Material> m_Material;

	class HealthComponent* m_HealthComponent;

	glm::vec3 m_LastPosition;

	glm::vec3 m_FacingDirection;

	float m_CurrentFireCooldown = 0;
	float m_FireCooldown = 0.45f;

	float m_CurrentInvincibleCooldown = 0;
	float m_InvincibleCooldown = 2.0f;
};


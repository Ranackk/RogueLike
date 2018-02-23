#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "LightComponent.h"

class PlayerComponent : public UpdateComponent
{
public:
	PlayerComponent();
	void initialize(Scene* _scene);
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

	float m_FireCooldown = 0;
	float m_FireCooldownDuration = 0.35f;

	float m_InvincibleCooldown = 0;
	float m_InvincibleCooldownDuration = 2.0f;


	static const int JOYSTICK_MOVE_AXIS_X = 0;
	static const int JOYSTICK_MOVE_AXIS_Y = 1;

	static const float JOYSTICK_MOVE_THRESHHOLD_X;
	static const float JOYSTICK_MOVE_THRESHHOLD_Y;
	static const int JOYSTICK_SHOOT_AXIS_X = 2;
	static const int JOYSTICK_SHOOT_AXIS_Y = 3;

	static const int JOYSTICK_LEFT_BUTTON = 2;
	static const int JOYSTICK_RIGHT_BUTTON = 1;
	static const int JOYSTICK_DOWN_BUTTON = 0;
	static const int JOYSTICK_UP_BUTTON = 3;
};


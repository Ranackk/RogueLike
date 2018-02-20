#pragma once
#include "GameObject.h"
#include "Material.h"

class CameraComponent : public UpdateComponent
{
public:
	enum Mode {
		FREE, SMOOTH_FOLLOW, FOLLOW, FOLLOW_ROOM,
		LOCKED
	};

	glm::mat4x4 m_ProjectionMatrix;
	glm::mat4x4 m_ViewMatrix;
	// View & Projection Matrix

	CameraComponent();
	void initialize(Mode _mode, class PlayerComponent* _player);
	~CameraComponent();
	void setMode(GLFWwindow* window, const Mode _mode);
	Mode getMode() const { return m_Mode;  }
	void update(GLFWwindow* window, const float ellapsed) override;

	glm::mat4 getViewMatrix() const;

	// Getter for matrices

private:
	float m_HorizontalAngle = 2.5f;
	float m_VerticalAngle = -1.0f;
	float m_InitialFOV = 45.0f;

	/* Mode: Free */
	float m_MouseSpeed = 0.005f;
	float m_Speed = 5.0f;

	/* Mode: Follow */
	class PlayerComponent* m_Player;
	glm::vec3 m_Offset;
	glm::vec3 m_DesiredPosition;
	float m_FollowSpeed = 5.0f;

	Mode m_Mode = Mode::FOLLOW_ROOM;
	Mode m_ModePreLock = Mode::FOLLOW_ROOM;

	void checkForCameraModeChange(GLFWwindow* window);
	void performFreeMovementUpdate(GLFWwindow* window, const float ellapesd);
	void performSmoothFollowMovementUpdate(GLFWwindow* window, const float ellapesd);
	void performFollowMovementUpdate(GLFWwindow* window, const float _ellapsed);
	void performFollowRoomMovementUpdate(GLFWwindow* window, const float ellapesd);


	/* Input */
	int m_LastStateEscape = -1;
};


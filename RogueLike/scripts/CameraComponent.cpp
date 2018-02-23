#include "stdafx.h"
#include "CameraComponent.h"
#include <gtc/matrix_transform.hpp>
#include "Transform.h"
#include "Game.h"




CameraComponent::CameraComponent()
{

}

void CameraComponent::initialize(const Mode _mode, PlayerComponent* _player) {
	this->m_Mode = _mode;
	this->m_Player = _player;

	const glm::vec3 startPos = Game::getInstance()->getCurrentScene()->getStartingPoint();
	this->m_GameObject->getTransform().setLocalPosition(glm::vec3(startPos.x, -10, startPos.z) + m_Offset);
	m_DesiredPosition = startPos + m_Offset;
	//= Transform(glm::vec3(25, 20, 30), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));

	//this->m_ProjectionMatrix = glm::ortho(0, 1, 0, 1, -20, 100);
	this->m_ProjectionMatrix = glm::perspective(
		glm::radians(60.0f),
		Game::m_s_cWindowWidth / (float)Game::m_s_cWindowHeight,
		Game::m_s_cNearClip,
		Game::m_s_cFarClip
	);
	//m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)Game::m_s_cWindowWidth / Game::m_s_cWindowHeight, Game::m_s_cNearClip, Game::m_s_cFarClip);

	this->m_ViewMatrix = glm::lookAt(
		this->m_GameObject->getTransform().getLocalPosition(),			// position
		glm::vec3(25, 0, 19.999f),										// focus point
		glm::vec3(0, 1, 0)												// up vector
										//glm::vec3(60, 100, 30),			// position
										//glm::vec3(60, 0, 29.999f),		// focus point
										//glm::vec3(0, 1, 0)				// up vector
	);

	this->m_Offset = glm::vec3(0, 15, 0);

}

CameraComponent::~CameraComponent() {
}

void CameraComponent::setMode(GLFWwindow* window, const Mode _mode) {
	m_Mode = _mode;

	switch (_mode) {
	case LOCKED: 
		glfwSetInputMode(window, GLFW_CURSOR,  GLFW_CURSOR_NORMAL);
	default:
		glfwSetCursorPos(window, Game::m_s_cWindowWidth / 2, Game::m_s_cWindowHeight / 2);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}

	//std::cout << "Set camera mode to " << (_mode == Mode::FREE ? "FREE" : (_mode == Mode::FOLLOW ? "FOLLOW" : _mode == Mode::FOLLOW_ROOM ? "FOLLOW_ROOM" : "LOCKED")) <<std::endl;
}

void CameraComponent::update(GLFWwindow* window ,const float ellapsed)
{
	checkForCameraModeChange(window);
	switch (m_Mode) {
		case LOCKED: break;
		case FREE: performFreeMovementUpdate(window, ellapsed); break;
		case FOLLOW: performFollowMovementUpdate(window, ellapsed); break;
		case SMOOTH_FOLLOW: performSmoothFollowMovementUpdate(window, ellapsed); break;
		case FOLLOW_ROOM: performFollowRoomMovementUpdate(window, ellapsed); break;
	}

}

void CameraComponent::checkForCameraModeChange(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		setMode(window, Mode::FOLLOW_ROOM);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		setMode(window, Mode::FOLLOW);
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		setMode(window, Mode::SMOOTH_FOLLOW);
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		setMode(window, Mode::FREE);
	}

	const int _currentStateEscape = glfwGetKey(window, GLFW_KEY_ESCAPE);
	if (_currentStateEscape == GLFW_PRESS && m_LastStateEscape != _currentStateEscape) {
		if (m_Mode == Mode::LOCKED) setMode(window, m_ModePreLock);
		else {
			m_ModePreLock = m_Mode;
			setMode(window, Mode::LOCKED);
		}
		m_LastStateEscape = _currentStateEscape;
	}
	if (_currentStateEscape == GLFW_RELEASE) {
		m_LastStateEscape = _currentStateEscape;
	}
}

glm::mat4x4 CameraComponent::getViewMatrix() const
{
	return m_ViewMatrix;
}

void CameraComponent::performFreeMovementUpdate(GLFWwindow* window, const float _ellapsed) {
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	glfwSetCursorPos(window, Game::m_s_cWindowWidth / 2, Game::m_s_cWindowHeight / 2);

	m_HorizontalAngle += m_MouseSpeed * float(Game::m_s_cWindowWidth / 2 - xPos);
	m_VerticalAngle += m_MouseSpeed * float(Game::m_s_cWindowHeight / 2 - yPos);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(m_VerticalAngle) * sin(m_HorizontalAngle),
		sin(m_VerticalAngle),
		cos(m_VerticalAngle) * cos(m_HorizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(m_HorizontalAngle - 3.14f / 2.0f),
		0,
		cos(m_HorizontalAngle - 3.14f / 2.0f)
	);

	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	Transform _transform = this->m_GameObject->getTransform();

	float moveSpeed = m_Speed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) moveSpeed *= 3;
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		_transform.setLocalPosition(_transform.getLocalPosition() + direction * _ellapsed * moveSpeed);
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		_transform.setLocalPosition(_transform.getLocalPosition() - direction * _ellapsed * moveSpeed);
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		_transform.setLocalPosition(_transform.getLocalPosition() - right * _ellapsed * moveSpeed);
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		_transform.setLocalPosition(_transform.getLocalPosition() + right * _ellapsed * moveSpeed);
	}
	// UP
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		_transform.setLocalPosition(_transform.getLocalPosition() + up * _ellapsed * moveSpeed);
	}
	// DOWN
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		_transform.setLocalPosition(_transform.getLocalPosition() - up * _ellapsed * moveSpeed);
	}
	m_GameObject->getTransform().setLocalPosition(_transform.getLocalPosition());

	m_ViewMatrix = glm::lookAt(
		_transform.getLocalPosition(),           
		_transform.getLocalPosition() + direction, 
		up                 
	);

}
void CameraComponent::performSmoothFollowMovementUpdate(GLFWwindow* window, const float _ellapsed) {
	m_DesiredPosition = m_Player->getGameObject()->getTransform().getPosition() + m_Offset;
	m_GameObject->getTransform().setLocalPosition(glm::mix(m_GameObject->getTransform().getLocalPosition(), m_DesiredPosition, _ellapsed * m_FollowSpeed));

	m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)Game::m_s_cWindowWidth / Game::m_s_cWindowHeight, Game::m_s_cNearClip, Game::m_s_cFarClip);

	m_ViewMatrix = glm::lookAt(
		m_GameObject->getTransform().getPosition(),					
		m_Player->getGameObject()->getTransform().getPosition(),	
		glm::vec3(0, 0, -1)											
	);	
}

void CameraComponent::performFollowMovementUpdate(GLFWwindow* window, const float _ellapsed) {
	m_DesiredPosition = m_Player->getGameObject()->getTransform().getPosition() + m_Offset;
	m_GameObject->getTransform().setLocalPosition(glm::mix(m_GameObject->getTransform().getLocalPosition(), m_DesiredPosition, _ellapsed * m_FollowSpeed));

	m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)Game::m_s_cWindowWidth / Game::m_s_cWindowHeight, Game::m_s_cNearClip, Game::m_s_cFarClip);

	m_ViewMatrix = glm::lookAt(
		m_GameObject->getTransform().getPosition(),
		m_GameObject->getTransform().getPosition() - m_Offset,
		glm::vec3(0, 0, -1)
	);
}

void CameraComponent::performFollowRoomMovementUpdate(GLFWwindow* window, const float _ellapsed) {
	const glm::vec3 roomMidPoint = Game::getInstance()->getCurrentScene()->getCurrentRoomMid();
	m_DesiredPosition = roomMidPoint + m_Offset;
	const glm::vec3 focusPoint = glm::mix(m_GameObject->getTransform().getLocalPosition(), roomMidPoint + m_Offset, _ellapsed * m_FollowSpeed) - m_Offset;

	m_GameObject->getTransform().setLocalPosition(glm::mix(m_GameObject->getTransform().getLocalPosition(), m_DesiredPosition, _ellapsed * m_FollowSpeed));

	m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)Game::m_s_cWindowWidth / Game::m_s_cWindowHeight, Game::m_s_cNearClip, Game::m_s_cFarClip);

	m_ViewMatrix = glm::lookAt(
		m_GameObject->getTransform().getPosition(),					
		focusPoint,													
		glm::vec3(0, 0, -1)											
	);

}

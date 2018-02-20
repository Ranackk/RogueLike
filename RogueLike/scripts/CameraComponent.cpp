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

	this->m_GameObject->getTransform().setLocalPosition(glm::vec3(25, 20, 30));
	//= Transform(glm::vec3(25, 20, 30), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));

	//this->m_ProjectionMatrix = glm::ortho(0, 1, 0, 1, -20, 100);
	this->m_ProjectionMatrix = glm::perspective(
		glm::radians(60.0f),
		Game::m_s_cWindowWidth / (float)Game::m_s_cWindowHeight,
		Game::m_s_cNearClip,
		Game::m_s_cFarClip
	);
	this->m_ViewMatrix = glm::lookAt(
		glm::vec3(25, 50, 20),			// position
		glm::vec3(25, 0, 19.999f),		// focus point
		glm::vec3(0, 1, 0)				// up vector
										//glm::vec3(60, 100, 30),			// position
										//glm::vec3(60, 0, 29.999f),		// focus point
										//glm::vec3(0, 1, 0)				// up vector
	);

	this->m_DesiredOffset = glm::vec3(0, 20, 5);
}

CameraComponent::~CameraComponent() {
}

void CameraComponent::setMode(GLFWwindow* window, const Mode _mode) {
	m_Mode = _mode;

	switch (_mode) {
	case LOCKED: 
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	default:
		glfwSetCursorPos(window, Game::m_s_cWindowWidth / 2, Game::m_s_cWindowHeight / 2);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
}

void CameraComponent::update(GLFWwindow* window ,const float ellapesd)
{
	switch (m_Mode) {
		case LOCKED: break;
		case FREE: performFreeMovementUpdate(window, ellapesd);
		case FOLLOW: performFollowMovementUpdate(window, ellapesd);
	}
}

glm::mat4x4 CameraComponent::getViewMatrix() const
{
	return m_ViewMatrix;
	//return this->m_Transform.getObjectMatrix();
	//return transform->GetObjectMatrix();
}

void CameraComponent::performFreeMovementUpdate(GLFWwindow* window, const float ellapesd) {
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
		_transform.setLocalPosition(_transform.getLocalPosition() + direction * ellapesd * moveSpeed);
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		_transform.setLocalPosition(_transform.getLocalPosition() - direction * ellapesd * moveSpeed);
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		_transform.setLocalPosition(_transform.getLocalPosition() - right * ellapesd * moveSpeed);
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		_transform.setLocalPosition(_transform.getLocalPosition() + right * ellapesd * moveSpeed);
	}
	// UP
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		_transform.setLocalPosition(_transform.getLocalPosition() + up * ellapesd * moveSpeed);
	}
	// DOWN
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		_transform.setLocalPosition(_transform.getLocalPosition() - up * ellapesd * moveSpeed);
	}
	m_GameObject->getTransform().setLocalPosition(_transform.getLocalPosition());

	// Projection matrix : 45° FieldComponent of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)Game::m_s_cWindowWidth / Game::m_s_cWindowHeight, Game::m_s_cNearClip, Game::m_s_cFarClip);
	// CameraComponent matrix
	m_ViewMatrix = glm::lookAt(
		_transform.getLocalPosition(),           // CameraComponent is here
		_transform.getLocalPosition() + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

	//std::cout << "CameraComponent Direction: " << direction.x << ", " << direction.y << ", " << direction.z << std::endl;
	//std::cout << "CameraComponent UP: " << up.x << ", " << up.y << ", " << up.z << std::endl;
}
void CameraComponent::performFollowMovementUpdate(GLFWwindow* window, const float ellapesd) {
	//const glm::vec3 desiredPosition = m_Player->getGameObject()->getTransform().getPosition() + m_DesiredOffset;
	//
	//m_GameObject->getTransform().setLocalPosition(glm::mix(m_GameObject->getTransform().getLocalPosition(), desiredPosition, ellapesd));

	//m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)Game::m_s_cWindowWidth / Game::m_s_cWindowHeight, Game::m_s_cNearClip, Game::m_s_cFarClip);

	//m_ViewMatrix = glm::lookAt(
	//	m_GameObject->getTransform().getPosition(),					// CameraComponent is here
	//	m_Player->getGameObject()->getTransform().getPosition(),	// and looks here : at the same position, plus "direction"
	//	glm::vec3(0,1,0)											// Head is up (set to 0,-1,0 to look upside-down)
	//);

}
#include "stdafx.h"
#include "Camera.h"
#include <gtc/matrix_transform.hpp>
#include "Transform.h"
#include "Game.h"

const float Game::m_s_cNearClip = 0.01f;
const float Game::m_s_cFarClip = 70.0f;


Camera::Camera()
{

}

void Camera::initialize() {
	this->m_Transform = Transform(glm::vec3(25, 20, 30), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
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
}

Camera::~Camera() {
}

void Camera::update(GLFWwindow* window ,const float ellapesd)
{
	// TODO: Move actual input to another class
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	glfwSetCursorPos(window, Game::m_s_cWindowWidth / 2, Game::m_s_cWindowHeight / 2);

	horizontalAngle += mouseSpeed * float(Game::m_s_cWindowWidth	/ 2 - xPos);
	verticalAngle	+= mouseSpeed * float(Game::m_s_cWindowHeight	/ 2 - yPos);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	//std::cout << horizontalAngle << ", " << verticalAngle << std::endl;
	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	float moveSpeed = speed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) moveSpeed *= 3;
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		m_Transform.setLocalPosition(m_Transform.getLocalPosition() + direction * ellapesd * moveSpeed);
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		m_Transform.setLocalPosition(m_Transform.getLocalPosition() - direction * ellapesd * moveSpeed);
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		m_Transform.setLocalPosition(m_Transform.getLocalPosition() - right * ellapesd * moveSpeed);
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		m_Transform.setLocalPosition(m_Transform.getLocalPosition() + right * ellapesd * moveSpeed);
	}
	// UP
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		m_Transform.setLocalPosition(m_Transform.getLocalPosition() + up * ellapesd * moveSpeed);
	}
	// DOWN
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		m_Transform.setLocalPosition(m_Transform.getLocalPosition() - up * ellapesd * moveSpeed);
	}

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)Game::m_s_cWindowWidth / Game::m_s_cWindowHeight, Game::m_s_cNearClip, Game::m_s_cFarClip);
	// Camera matrix
	m_ViewMatrix = glm::lookAt(
		m_Transform.getLocalPosition(),           // Camera is here
		m_Transform.getLocalPosition() + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

	//std::cout << "Camera Direction: " << direction.x << ", " << direction.y << ", " << direction.z << std::endl;
	//std::cout << "Camera UP: " << up.x << ", " << up.y << ", " << up.z << std::endl;
}

glm::mat4x4 Camera::getViewMatrix() const
{
	return m_ViewMatrix;
	//return this->m_Transform.getObjectMatrix();
	//return transform->GetObjectMatrix();
}

#pragma once
#include "GameObject.h"
#include "Material.h"

class CameraComponent : public UpdateComponent
{
	//static const int WIDTH = 640;
	//static const int HEIGHT = 480;

public:
	glm::mat4x4 m_ProjectionMatrix;
	glm::mat4x4 m_ViewMatrix;
	// View & Projection Matrix

	CameraComponent();
	void initialize();
	~CameraComponent();

	void update(GLFWwindow* window, const float ellapsed) override;

	glm::mat4 getViewMatrix() const;

	// Getter for matrices

private:
	float horizontalAngle = 2.5f;
	float verticalAngle = -1.0f;
	float initialFOV = 45.0f;

	float mouseSpeed = 0.005f;
	float speed = 5.0f;
};


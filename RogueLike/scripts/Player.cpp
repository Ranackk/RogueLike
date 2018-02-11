#include "stdafx.h"
#include "Player.h"
#include "Game.h"
#include <string>
#include "ColliderComponent.h"
#include "CircleCollider.h"
#include "CircleColliderComponent.h"
#include "PhysicsEngine.h"


Player::Player()
{
}

void Player::initialize() {
	const float rad = 0.1f;
	this->m_MovementSpeed = 4.0f;
	this->m_Transform.setLocalPosition(glm::vec3(19.5, 0, 13.5));
	this->m_Transform.setLocalScale(glm::vec3(rad * 2,1.5, rad * 2));

	/* Create Render Component */
	const GLuint texture = Game::getInstance()->getTextureManager()->getTextureByIdentifier("tex_Player");

	std::shared_ptr<Material> material = Game::getInstance()->getMaterialManager()->getMaterialByName("mat_Player");
	material->setupBaseShader(glm::vec4(1, 1, 1, 1), texture, Game::getInstance()->getMaterialManager()->m_Skybox);

	const std::shared_ptr<ModelData> modelData = Game::getInstance()->getModelManager()->getModelDataByIdentifier("mesh_Player");

	RenderComponent* rc = addComponent<>(new RenderComponent());
	rc->initialize(modelData, material);

	CircleColliderComponent* cc = addComponent<>(new CircleColliderComponent());
	CircleCollider circC = CircleCollider(rad, glm::vec3(0, 0, 0));
	circC.initialize(std::shared_ptr<Player>(this));
	circC.setCollisionLayer(CollisionLayer::FRIENDLY);
	cc->initialize(circC);

	m_LightOffset = glm::vec3(0, 1, 0);

	/* Add Light */
	m_Light = new Light();
	m_Light->initialize(glm::vec3(), 30, glm::vec4(0.5, 0.5, 2.5, 1), Light::DYNAMIC, false);
	//m_Light.getTransform().setParent(&m_Transform);		// TODO: Implement Parenting System
	m_Transform.addChildTransform(&m_Light->getTransform());
	m_Light->getTransform().setLocalPosition(m_LightOffset);
}

// TODO: Add scene to update method
void Player::update(GLFWwindow* window, const float deltaTime) {

	/* Up / Down Movement */
	glm::vec3 movementVector = glm::vec3();
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		//m_Transform.translate(-glm::vec3(0, 0, 1) * deltaTime * m_MovementSpeed);
		movementVector += -glm::vec3(0, 0, 1) * deltaTime * m_MovementSpeed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		//m_Transform.translate(+glm::vec3(0, 0, 1) * deltaTime * m_MovementSpeed);
		movementVector += glm::vec3(0, 0, 1) * deltaTime * m_MovementSpeed;
	}

	m_Transform.setLocalPosition(m_Transform.getLocalPosition() + movementVector);
	if (Game::getInstance()->getCurrentScene()->collidesWithSceneGeometry(getComponent<CircleColliderComponent>()->getCollider())) {
		m_Transform.setLocalPosition(m_Transform.getLocalPosition() - movementVector);
	}

	/* Left / Right Movement */
	movementVector = glm::vec3();
	// Move left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		//m_Transform.translate(-glm::vec3(1, 0, 0) * deltaTime * m_MovementSpeed);
		movementVector += -glm::vec3(1, 0, 0) * deltaTime * m_MovementSpeed;
	}
	// Move right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		//m_Transform.translate(+ glm::vec3(1, 0, 0) * deltaTime * m_MovementSpeed);
		movementVector += glm::vec3(1, 0, 0) * deltaTime * m_MovementSpeed;
	}

	m_Transform.setLocalPosition(m_Transform.getLocalPosition() + movementVector);
	if (Game::getInstance()->getCurrentScene()->collidesWithSceneGeometry(getComponent<CircleColliderComponent>()->getCollider())) {
		m_Transform.setLocalPosition(m_Transform.getLocalPosition() - movementVector);
	}


	//std::cout << "Player Position: " << m_Transform.getPosition().x << ", " << m_Transform.getPosition().y << ", " << m_Transform.getPosition().z <<
		//" ||| Light Pos: " << m_Light->getTransform().getPosition().x << ", " << m_Light->getTransform().getPosition().y << ", " << m_Light->getTransform().getPosition().z << std::endl;
}

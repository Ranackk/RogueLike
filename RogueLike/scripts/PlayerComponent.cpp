#include "stdafx.h"
#include "PlayerComponent.h"
#include "Game.h"
#include <string>
#include "ColliderComponent.h"
#include "CircleCollider.h"
#include "CircleColliderComponent.h"
#include "PhysicsEngine.h"
#include "Projectile.h"


PlayerComponent::PlayerComponent()
{
}

void PlayerComponent::initialize() {
	const float rad = 0.1f;
	this->m_MovementSpeed = 4.0f;
	this->m_GameObject->getTransform().setLocalPosition(glm::vec3(19.5, 0, 13.5));
	this->m_GameObject->getTransform().setLocalScale(glm::vec3(rad * 2,1.5, rad * 2));

	/* Create Render Component */
	const GLuint texture = Game::getInstance()->getTextureManager()->getTextureByIdentifier("tex_Player");
	std::shared_ptr<Material> material = Game::getInstance()->getMaterialManager()->getMaterialByName("mat_Player");
	const std::shared_ptr<ModelData> modelData = Game::getInstance()->getModelManager()->getModelDataByIdentifier("mesh_Player");

	material->setTexture(texture);

	RenderComponent* rc = m_GameObject->addComponent<>(new RenderComponent());
	rc->initialize(modelData, material);

	CircleColliderComponent* cc = m_GameObject-> addComponent<>(new CircleColliderComponent());
	CircleCollider circC = CircleCollider(rad, glm::vec3(0, 0, 0));
	circC.initialize(std::shared_ptr<GameObject>(m_GameObject));
	circC.setCollisionLayer(CollisionLayer::FRIENDLY_UNITS);
	cc->initialize(circC);

	m_LightOffset = glm::vec3(0, 1, 0);

	/* Add LightComponent */
	GameObject* gO = new GameObject("Player Light");
	m_Light = gO->addComponent(new LightComponent());
	m_Light->initialize(glm::vec3(), 30, glm::vec4(0.5, 0.5, 2.5, 1), LightComponent::DYNAMIC, false);
	//m_Light.getTransform().setParent(&m_Transform);		// TODO: Implement Parenting System
	m_GameObject->getTransform().addChildTransform(&m_Light->getGameObject()->getTransform());
	m_Light->getGameObject()->getTransform().setLocalPosition(m_LightOffset);

}

// TODO: Add scene to update method
void PlayerComponent::update(GLFWwindow* window, const float deltaTime) {

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

	m_GameObject->getTransform().setLocalPosition(m_GameObject->getTransform().getLocalPosition() + movementVector);
	if (Game::getInstance()->getCurrentScene()->collidesWithSceneGeometry(m_GameObject->getComponent<CircleColliderComponent>()->getCollider())) {
		m_GameObject->getTransform().setLocalPosition(m_GameObject->getTransform().getLocalPosition() - movementVector);
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

	m_GameObject->getTransform().setLocalPosition(m_GameObject->getTransform().getLocalPosition() + movementVector);
	if (Game::getInstance()->getCurrentScene()->collidesWithSceneGeometry(m_GameObject->getComponent<CircleColliderComponent>()->getCollider())) {
		m_GameObject->getTransform().setLocalPosition(m_GameObject->getTransform().getLocalPosition() - movementVector);
	}

	/* Shooting Stars */
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		GameObject* bullet = Game::getInstance()->getCurrentScene()->m_ProjectilePool.getNextFreeObject();
		Projectile* projectileComponent = bullet->getComponent<Projectile>();
		if (projectileComponent == nullptr) {
			projectileComponent = bullet->addComponent(new Projectile);
		}
		projectileComponent->initialize(m_GameObject->getTransform().getPosition(), glm::vec3(1, 0, 1), .1f, CollisionLayer::FRIENDLY_UNITS);
		Game::getInstance()->getCurrentScene()->m_ProjectilePool.updateRenderBatch();
	}
	//std::cout << "PlayerComponent Position: " << m_Transform.getPosition().x << ", " << m_Transform.getPosition().y << ", " << m_Transform.getPosition().z <<
		//" ||| LightComponent Pos: " << m_Light->getTransform().getPosition().x << ", " << m_Light->getTransform().getPosition().y << ", " << m_Light->getTransform().getPosition().z << std::endl;
}

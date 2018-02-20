#include "stdafx.h"
#include "PlayerComponent.h"
#include "Game.h"
#include <string>
#include "ColliderComponent.h"
#include "CircleCollider.h"
#include "CircleColliderComponent.h"
#include "PhysicsEngine.h"
#include "ProjectileComponent.h"
#include "HealthComponent.h"


PlayerComponent::PlayerComponent()
{
}

void PlayerComponent::initialize() {
	const float rad = 0.1f;
	this->m_MovementSpeed = 4.0f;
	this->m_GameObject->getTransform().setLocalPosition(glm::vec3(19.5, 0, 13.5));
	this->m_GameObject->getTransform().setLocalScale(glm::vec3(rad * 2,1.5, rad * 2));
	m_FacingDirection = glm::vec3(1, 0, 0);

	/* Add Render Component */
	const GLuint texture = Game::getInstance()->getTextureManager()->getTextureByIdentifier("tex_Player");
	m_Material = Game::getInstance()->getMaterialManager()->getMaterialByName("mat_Player");
	const std::shared_ptr<ModelData> modelData = Game::getInstance()->getModelManager()->getModelDataByIdentifier("mesh_Player");
	m_Material->setTexture(texture);

	RenderComponent* rc = m_GameObject->addComponent<>(new RenderComponent());
	rc->initialize(modelData, m_Material);

	/* Add Collider Component*/
	CircleColliderComponent* cc = m_GameObject-> addComponent<>(new CircleColliderComponent());
	CircleCollider circC = CircleCollider(rad, glm::vec3(0, 0, 0));
	circC.initialize(std::shared_ptr<GameObject>(m_GameObject));
	circC.setCollisionLayer(CollisionLayer::FRIENDLY_UNITS);
	cc->initialize(circC);

	m_LightOffset = glm::vec3(0, 1, 0);

	/* Add child gameobject with LightComponent */
	GameObject* gO = new GameObject("Player Light");
	m_Light = gO->addComponent(new LightComponent());
	m_Light->initialize(glm::vec3(), 30, glm::vec4(0.5, 0.5, 1, 1), LightComponent::DYNAMIC, false);
	m_GameObject->getTransform().addChildTransform(&m_Light->getGameObject()->getTransform());
	m_Light->getGameObject()->getTransform().setLocalPosition(m_LightOffset);

	/* Add Health Component */
	m_HealthComponent = m_GameObject->addComponent(new HealthComponent());
	m_HealthComponent->initialize(5, 5);

}

void PlayerComponent::takeDamage(const float _amount) {
	if (m_CurrentInvincibleCooldown <= 0) {
		m_HealthComponent->takeDamage(_amount);
		std::cout << m_GameObject->getName().c_str() << " took " << _amount << " damage! NEW HP: " << std::to_string((*m_HealthComponent->getCurrentHealthPointer())) << std::endl;
		m_CurrentInvincibleCooldown = m_InvincibleCooldown;
	}
	
}

// TODO: Add scene to update method
void PlayerComponent::update(GLFWwindow* window, const float deltaTime) {

	/* Up / Down Movement */
	glm::vec3 movementVector = glm::vec3();
	glm::vec3 combinedVector = glm::vec3();
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		movementVector += -glm::vec3(0, 0, 1) * deltaTime * m_MovementSpeed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		movementVector += glm::vec3(0, 0, 1) * deltaTime * m_MovementSpeed;
	}

	EnemyComponent* enemyComponent = nullptr;
	m_GameObject->getTransform().setLocalPosition(m_GameObject->getTransform().getLocalPosition() + movementVector);
	if (Game::getInstance()->getCurrentScene()->collidesWithEnemies(m_GameObject->getComponent<CircleColliderComponent>()->getCollider(), enemyComponent)) {
		takeDamage(0.5f);
		m_GameObject->getTransform().setLocalPosition(m_GameObject->getTransform().getLocalPosition() - movementVector);
	}
	else if (Game::getInstance()->getCurrentScene()->collidesWithSceneGeometry(m_GameObject->getComponent<CircleColliderComponent>()->getCollider())) {
		m_GameObject->getTransform().setLocalPosition(m_GameObject->getTransform().getLocalPosition() - movementVector);
	}

	/* Left / Right Movement */
	combinedVector += movementVector;
	movementVector = glm::vec3();
	// Move left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		movementVector += -glm::vec3(1, 0, 0) * deltaTime * m_MovementSpeed;
	}
	// Move right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		movementVector += glm::vec3(1, 0, 0) * deltaTime * m_MovementSpeed;
	}

	m_GameObject->getTransform().setLocalPosition(m_GameObject->getTransform().getLocalPosition() + movementVector);
	if (Game::getInstance()->getCurrentScene()->collidesWithEnemies(m_GameObject->getComponent<CircleColliderComponent>()->getCollider(), enemyComponent)) {
		takeDamage(0.5f);
		m_GameObject->getTransform().setLocalPosition(m_GameObject->getTransform().getLocalPosition() - movementVector);
	}
	else if (Game::getInstance()->getCurrentScene()->collidesWithSceneGeometry(m_GameObject->getComponent<CircleColliderComponent>()->getCollider())) {
		m_GameObject->getTransform().setLocalPosition(m_GameObject->getTransform().getLocalPosition() - movementVector);
	}


	combinedVector += movementVector;
	if (combinedVector != glm::vec3()) {
		m_FacingDirection = combinedVector;
	}

	/* Shooting Stars */
	m_CurrentFireCooldown -= deltaTime;
	if (m_CurrentFireCooldown <= 0) {
		bool shoot = false;
		glm::vec3 dir;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			shoot = true;
			dir = glm::vec3(-1, 0, 0);
		}
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			shoot = true;
			dir = glm::vec3(1, 0, 0);
		}
		else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			shoot = true;
			dir = glm::vec3(0, 0, -1);
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			shoot = true;
			dir = glm::vec3(0, 0, 1);
		}
		if (shoot){
			GameObject* bullet = Game::getInstance()->getCurrentScene()->m_ProjectilePool.getNextFreeObject();
			ProjectileComponent* projectileComponent = bullet->getComponent<ProjectileComponent>();
			if (projectileComponent == nullptr) {
				projectileComponent = bullet->addComponent(new ProjectileComponent);
			}
			projectileComponent->initialize(m_GameObject->getTransform().getPosition() + glm::vec3(0, 0, 0), dir + combinedVector, .15f, CollisionLayer::FRIENDLY_UNITS);
			Game::getInstance()->getCurrentScene()->m_ProjectilePool.updateRenderBatch();

			m_CurrentFireCooldown = m_FireCooldown;
		}
	}
	/*if (m_CurrentFireCooldown <= 0 && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		GameObject* bullet = Game::getInstance()->getCurrentScene()->m_ProjectilePool.getNextFreeObject();
		ProjectileComponent* projectileComponent = bullet->getComponent<ProjectileComponent>();
		if (projectileComponent == nullptr) {
			projectileComponent = bullet->addComponent(new ProjectileComponent);
		}
		projectileComponent->initialize(m_GameObject->getTransform().getPosition(), m_FacingDirection, .1f, CollisionLayer::FRIENDLY_UNITS);
		Game::getInstance()->getCurrentScene()->m_ProjectilePool.updateRenderBatch();

		m_CurrentFireCooldown = m_FireCooldown;
	}*/

	/* === Flashing === */
	if (m_CurrentInvincibleCooldown != 0.0f) {
		m_CurrentInvincibleCooldown -= deltaTime;
		static const float AMT_FLASHES = 3;
		const float flashFactor = -glm::cos(m_CurrentInvincibleCooldown * (2 * 3.14f * AMT_FLASHES / m_InvincibleCooldown)) / 2.0f + 0.5f;
		m_Material->setDiffuse(glm::vec4(1, flashFactor, flashFactor, 1));

		if (m_CurrentInvincibleCooldown < 0.0f) {
			m_CurrentInvincibleCooldown = 0.0f;
			m_Material->setDiffuse(glm::vec4(1, 1, 1, 1));
		}
	}

	//std::cout << "PlayerComponent Position: " << m_Transform.getPosition().x << ", " << m_Transform.getPosition().y << ", " << m_Transform.getPosition().z <<
		//" ||| LightComponent Pos: " << m_Light->getTransform().getPosition().x << ", " << m_Light->getTransform().getPosition().y << ", " << m_Light->getTransform().getPosition().z << std::endl;
}

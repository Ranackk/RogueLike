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

const float PlayerComponent::JOYSTICK_MOVE_THRESHHOLD_X = 0.65f;
const float PlayerComponent::JOYSTICK_MOVE_THRESHHOLD_Y = 0.5f;

PlayerComponent::PlayerComponent() {}

void PlayerComponent::initialize(Scene* _scene) {
	const float rad = 0.1f;
	this->m_MovementSpeed = 4.0f;
	this->m_GameObject->getTransform().setLocalPosition(_scene->getStartingPoint());
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
	m_HealthComponent->initialize(5);

}

void PlayerComponent::takeDamage(const float _amount) {
	if (m_InvincibleCooldown <= 0) {
		m_HealthComponent->takeDamage(_amount);
		std::cout << m_GameObject->getName().c_str() << " took " << _amount << " damage! NEW HP: " << std::to_string((*m_HealthComponent->getCurrentHealthPointer())) << std::endl;
		m_InvincibleCooldown = m_InvincibleCooldownDuration;
	}
	
}

void PlayerComponent::update(GLFWwindow* window, const float deltaTime) {
	const bool joystickActive = glfwJoystickPresent(GLFW_JOYSTICK_1) == 1;
	
	int joystickAxesCount;
	const float* joystickAxesValues = nullptr;
	int joystickButtonCount;
	const unsigned char* joystickButtonValues = nullptr;
	if (joystickActive ){
		joystickAxesValues = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &joystickAxesCount);
		joystickButtonValues = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &joystickButtonCount);
	}

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
	/* Joystick Input*/
	if (joystickActive) {
		movementVector += glm::vec3(0, 0, -1) * deltaTime * m_MovementSpeed * joystickAxesValues[JOYSTICK_MOVE_AXIS_Y];
	}

	EnemyComponent* enemyComponent = nullptr;
	m_GameObject->getTransform().setLocalPosition(m_GameObject->getTransform().getLocalPosition() + movementVector);
	if (m_InvincibleCooldown <= 0.0f && Game::getInstance()->getCurrentScene()->collidesWithEnemies(m_GameObject->getComponent<CircleColliderComponent>()->getCollider(), enemyComponent)) {
		takeDamage(enemyComponent->getDamage(EnemyComponent::Range::MELEE));
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
	/* Joystick Input*/
	if (joystickActive) {
		movementVector += glm::vec3(1, 0, 0) * deltaTime * m_MovementSpeed * joystickAxesValues[JOYSTICK_MOVE_AXIS_X];
	}

	m_GameObject->getTransform().setLocalPosition(m_GameObject->getTransform().getLocalPosition() + movementVector);
	if (m_InvincibleCooldown <= 0.0f && Game::getInstance()->getCurrentScene()->collidesWithEnemies(m_GameObject->getComponent<CircleColliderComponent>()->getCollider(), enemyComponent)) {
		takeDamage(enemyComponent->getDamage(EnemyComponent::Range::MELEE));
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
	if (m_FireCooldown > 0) {
		m_FireCooldown -= deltaTime;
	}
	if (m_FireCooldown <= 0) {
		bool shoot = false;
		glm::vec3 dir;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || joystickActive && 
			(joystickAxesValues[JOYSTICK_SHOOT_AXIS_X] < -JOYSTICK_MOVE_THRESHHOLD_X || joystickButtonValues[JOYSTICK_LEFT_BUTTON]) != 0) {
			shoot = true;
			dir = glm::vec3(-1, 0, 0);
		}
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || joystickActive && 
			(joystickAxesValues[JOYSTICK_SHOOT_AXIS_X] > JOYSTICK_MOVE_THRESHHOLD_X || joystickButtonValues[JOYSTICK_RIGHT_BUTTON]) != 0) {
			shoot = true;
			dir = glm::vec3(1, 0, 0);
		}
		else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || joystickActive && 
			(joystickAxesValues[JOYSTICK_SHOOT_AXIS_Y] > JOYSTICK_MOVE_THRESHHOLD_Y || joystickButtonValues[JOYSTICK_UP_BUTTON]) != 0) {
			shoot = true;
			dir = glm::vec3(0, 0, -1);
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || joystickActive && 
			(joystickAxesValues[JOYSTICK_SHOOT_AXIS_Y] < -JOYSTICK_MOVE_THRESHHOLD_Y || joystickButtonValues[JOYSTICK_DOWN_BUTTON]) != 0) {
			shoot = true;
			dir = glm::vec3(0, 0, 1);
		}

		if (shoot){
			m_FireCooldown = m_FireCooldownDuration;

			GameObject* bullet = Game::getInstance()->getCurrentScene()->m_ProjectilePool.getNextFreeObject();
			ProjectileComponent* projectileComponent = bullet->getComponent<ProjectileComponent>();
			if (projectileComponent == nullptr) {
				projectileComponent = bullet->addComponent(new ProjectileComponent);
			}
			projectileComponent->initialize(m_GameObject->getTransform().getPosition() + glm::vec3(0, 0, 0), dir + combinedVector, .15f, 1.0f, CollisionLayer::FRIENDLY_UNITS);
			Game::getInstance()->getCurrentScene()->m_ProjectilePool.updateRenderBatch();

		}
	}


	/* === Flashing === */
	if (m_InvincibleCooldown != 0.0f) {
		m_InvincibleCooldown -= deltaTime;
		static const float AMT_FLASHES = 3;
		const float flashFactor = -glm::cos(m_InvincibleCooldown * (2 * 3.14f * AMT_FLASHES / m_InvincibleCooldownDuration)) / 2.0f + 0.5f;
		m_Material->setDiffuse(glm::vec4(1, flashFactor, flashFactor, 1));

		if (m_InvincibleCooldown < 0.0f) {
			m_InvincibleCooldown = 0.0f;
			m_Material->setDiffuse(glm::vec4(1, 1, 1, 1));
		}
	}

	//std::cout << "PlayerComponent Position: " << m_Transform.getPosition().x << ", " << m_Transform.getPosition().y << ", " << m_Transform.getPosition().z <<
		//" ||| LightComponent Pos: " << m_Light->getTransform().getPosition().x << ", " << m_Light->getTransform().getPosition().y << ", " << m_Light->getTransform().getPosition().z << std::endl;
}

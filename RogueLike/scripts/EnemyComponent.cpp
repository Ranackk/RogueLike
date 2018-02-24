#include "stdafx.h"
#include "EnemyComponent.h"
#include "Game.h"
#include "CircleColliderComponent.h"
#include "PhysicsEngine.h"
#include "HealthComponent.h"
#include <string>
#include "ProjectileComponent.h"
#include <gtc/matrix_transform.inl>


EnemyComponent::EnemyType EnemyComponent::getTypeByColor(int v) {
	switch (v) {
	case 255: return ROGUE;
	case 100: return ARCHER;
	default: return ARCHER;
	}
}

EnemyComponent::EnemyComponent()
{
	m_CircleCollider = nullptr;
	m_Scene = nullptr;
	m_HealthComponent = nullptr;
	m_Type = ROGUE;
}

void EnemyComponent::initialize(Scene* _scene, const EnemyType _type, const glm::vec3 _position) {
	/* Basic Variables */
	m_GameObject->getTransform().setLocalPosition(_position);
	m_Scene = _scene;
	m_Type = _type;
	m_GameObject->setActive(true);

	/* Shared AI Information */
	m_StartPosition = m_GameObject->getTransform().getPosition();
	m_RoomGridPos = _scene->getRoomGridPos(m_StartPosition);

	/* Type specific setup */
	switch (m_Type) {
		case ROGUE: initializeEnemyTypeRogue(); break;
		case ARCHER: initializeEnemyTypeArcher(); break;
	}
}


void EnemyComponent::initializeEnemyTypeRogue() {
	m_GameObject->getTransform().setLocalScale(glm::vec3(0.5f, 1.0f, 0.5));

	/* Render Component */
	const GLuint texture = Game::getInstance()->getTextureManager()->getTextureByIdentifier("tex_Enemy_Rogue");
	std::shared_ptr<Material> material = Game::getInstance()->getMaterialManager()->getMaterialByName("mat_Enemy_Rogue");
	const std::shared_ptr<ModelData> modelData = Game::getInstance()->getModelManager()->getModelDataByIdentifier("mesh_Enemy_Rogue");
	material->setTexture(texture);

	RenderComponent* rc = m_GameObject->addComponent<>(new RenderComponent());
	rc->initialize(modelData, material);

	/* Collider */
	CircleColliderComponent* cc = m_GameObject->addComponent<>(new CircleColliderComponent());
	m_CircleCollider = new CircleCollider(0.25f, glm::vec3(0, 0, 0));
	m_CircleCollider->initialize(std::shared_ptr<GameObject>(m_GameObject));
	m_CircleCollider->setCollisionLayer(CollisionLayer::HOSTILE_UNITS);
	cc->initialize(*m_CircleCollider);

	/* Add Health Component */
	m_HealthComponent = m_GameObject->addComponent(new HealthComponent());
	m_HealthComponent->initialize(m_RogueHealthPoints);
}
void EnemyComponent::initializeEnemyTypeArcher() {
	m_GameObject->getTransform().setLocalScale(glm::vec3(0.5f, 1.0f, 0.5));

	/* Render Component */
	const GLuint texture = Game::getInstance()->getTextureManager()->getTextureByIdentifier("tex_Enemy_Archer");
	std::shared_ptr<Material> material = Game::getInstance()->getMaterialManager()->getMaterialByName("mat_Enemy_Archer");
	const std::shared_ptr<ModelData> modelData = Game::getInstance()->getModelManager()->getModelDataByIdentifier("mesh_Enemy_Archer");
	material->setTexture(texture);

	RenderComponent* rc = m_GameObject->addComponent<>(new RenderComponent());
	rc->initialize(modelData, material);

	/* Collider */
	CircleColliderComponent* cc = m_GameObject->addComponent<>(new CircleColliderComponent());
	m_CircleCollider = new CircleCollider(0.25f, glm::vec3(0, 0, 0));
	m_CircleCollider->initialize(std::shared_ptr<GameObject>(m_GameObject));
	m_CircleCollider->setCollisionLayer(CollisionLayer::HOSTILE_UNITS);
	cc->initialize(*m_CircleCollider);

	/* Add Health Component */
	m_HealthComponent = m_GameObject->addComponent(new HealthComponent());
	m_HealthComponent->initialize(m_ArcherHealthPoints);
}

void EnemyComponent::update(GLFWwindow* _window, const float _deltaTime) {
	m_AliveTime += _deltaTime;

	switch (m_Type) {
		case ROGUE: updateEnemyTypRogue(_window, _deltaTime); break;
		case ARCHER: updateEnemyTypArcher(_window, _deltaTime); break;
	}
}

float EnemyComponent::getDamage(const Range _range) const {
	switch (m_Type) {
	case ROGUE: return m_RogueMeleeDamage;
	case ARCHER: if (_range == MELEE) return m_ArcherMeleeDamage;
		return m_ArcherRangedDamage;
	}
	return -1;
}


void EnemyComponent::takeDamage(const float _amount, const glm::vec3 _knockback) const {
	//std::cout << m_GameObject->getName().c_str() << " took " << _amount << " damage! NEW HP: " << std::to_string((*m_HealthComponent->getCurrentHealthPointer())) << std::endl;

	/* Damage */
	if (m_HealthComponent->takeDamage(_amount)) {
		die();
	}	

	/* Knockback */
	m_GameObject->getTransform().translate(_knockback);
	if (Game::getInstance()->getCurrentScene()->collidesWithSceneGeometry(*m_CircleCollider)) {
		m_GameObject->getTransform().translate(-glm::vec3(_knockback));
	}
}

void EnemyComponent::die() const {
	//std::cout << "EnemyComponent died!" << std::endl;
	m_GameObject->setActive(false);
	m_Scene->m_EnemyPools[m_Type].freeObjectIntoPool(m_GameObject);
}

void EnemyComponent::updateEnemyTypRogue(GLFWwindow* _window, const float _deltaTime) {
	//const glm::vec3 newPosition = m_StartPosition + 0.5f * glm::vec3(cos(m_AliveTime), 0.0f, sin(m_AliveTime));
	//m_GameObject->getTransform().setLocalPosition(newPosition);

	bool playerInMyRoom = m_Scene->isPlayerFullyInRoom(m_RoomGridPos);

	//const glm::vec2 _currentRoomGridPos = m_Scene->getCurrentRoomGridPos();
	const glm::vec3 _currentPosition = m_GameObject->getTransform().getPosition();

	glm::vec3 _desiredPosition;
	glm::vec3 _desiredVector;
	if (m_RogueCharging) {
		/* Check if the rogue should stop charging */
		if (!playerInMyRoom) {
			m_RogueCharging = false;
		}

		/* Target player */
		_desiredPosition = m_Scene->getPlayer()->getGameObject()->getTransform().getPosition();
		_desiredVector = _desiredPosition - _currentPosition;
	}
	else {
		/* Check if the rogue should start charging */
		if (playerInMyRoom) {
			m_RogueCharging = true;
		}

		/* Return to starting position */
		_desiredPosition = m_StartPosition;
		_desiredVector = _desiredPosition - _currentPosition;
	}



	/* Carry out movement */
	if (glm::length(_desiredVector) > 0.01f) {
		m_GameObject->getTransform().setLocalRotation(glm::lookAt(_currentPosition, _currentPosition + glm::vec3(-_desiredVector.x, 0, _desiredVector.z), glm::vec3(0, 1, 0)));
		const glm::vec3 movementVector = normalize(_desiredVector) * _deltaTime * m_RogueSpeed;

		//std::cout << glm::length(_desiredPosition) << " " <<movementVector.x << std::endl;
		m_GameObject->getTransform().translate(glm::vec3(movementVector.x, 0, 0));
		if (Game::getInstance()->getCurrentScene()->collidesWithSceneGeometry(*m_CircleCollider)) {
			m_GameObject->getTransform().translate(-glm::vec3(movementVector.x, 0, 0));
		}
		m_GameObject->getTransform().translate(glm::vec3(0, 0, movementVector.z));
		if (Game::getInstance()->getCurrentScene()->collidesWithSceneGeometry(*m_CircleCollider)) {
			m_GameObject->getTransform().translate(-glm::vec3(0, 0, movementVector.z));
		}
	}
	else {
		m_GameObject->getTransform().setLocalPosition(_desiredPosition);
	}
}

void EnemyComponent::updateEnemyTypArcher(GLFWwindow* _window, const float _deltaTime) {
	const glm::vec2 _currentRoomGridPos = m_Scene->getCurrentRoomGridPos();

	if (_currentRoomGridPos == m_RoomGridPos) {
		/* Try to shoot */
		/* Find vector to player */
		const glm::vec3 shootVector = glm::normalize(m_Scene->getPlayer()->getGameObject()->getTransform().getPosition() - m_GameObject->getTransform().getPosition());
		/* Aim*/
		const glm::vec3 pos = m_GameObject->getTransform().getPosition();
		m_GameObject->getTransform().setLocalRotation(glm::lookAt(pos, pos + glm::vec3(-shootVector.x, 0, shootVector.z), glm::vec3(0, 1, 0)));
		if (m_ArcherShootCooldown <= 0.0f) {
			/* Shoot */
			GameObject* bullet = Game::getInstance()->getCurrentScene()->m_ProjectilePool.getNextFreeObject();
			ProjectileComponent* projectileComponent = bullet->getComponent<ProjectileComponent>();
			if (projectileComponent == nullptr) {
				projectileComponent = bullet->addComponent(new ProjectileComponent);
			}
			projectileComponent->initialize(m_GameObject->getTransform().getPosition() + glm::vec3(0, 0, 0), shootVector, m_ArcherShootSpeed, m_ArcherRangedDamage, CollisionLayer::HOSTILE_UNITS);
			Game::getInstance()->getCurrentScene()->m_ProjectilePool.updateRenderBatch();
			/* Reset Cooldown */
			m_ArcherShootCooldown = m_ArcherShootCooldownDuration;
		}
		else {
			m_ArcherShootCooldown -= _deltaTime;
		}
	}
}
void EnemyComponent::setRoomCoord(const glm::vec2 _roomCoord)
{
	m_RoomCoord = _roomCoord;
}


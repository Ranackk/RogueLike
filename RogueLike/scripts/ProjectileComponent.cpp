#include "stdafx.h"
#include "ProjectileComponent.h"
#include "Scene.h"
#include "Game.h"
#include "CircleColliderComponent.h"
#include "PhysicsEngine.h"


ProjectileComponent::ProjectileComponent()
{
}

void ProjectileComponent::update(GLFWwindow* window, const float deltaTime) {
	m_GameObject->getTransform().translate(m_Direction * m_MovementSpeed * deltaTime);

	// Collision with Map
	if (Game::getInstance()->getCurrentScene()->collidesWithSceneGeometry(*m_CircleCollider, true)) {
		die();
	}

	// Collision with Enemies
	EnemyComponent* hit = nullptr;
	if (Game::getInstance()->getCurrentScene()->collidesWithEnemies(*m_CircleCollider, hit)) {
		hit->takeDamage(m_Damage, m_Direction * 0.3f);
		die();
	}

	// Collision with PlayerComponent
	if (Game::getInstance()->getCurrentScene()->collidesWithPlayer(*m_CircleCollider)) {
		Game::getInstance()->getCurrentScene()->getPlayer()->takeDamage(m_Damage);
		die();
	}


	// Generic approach: Check for every collision, then send collision events such as:
	//hit.getComponent<ColliderComponent>()->fireCollision(*m_CircleColliderComponent);
	//m_CircleColliderComponent->fireCollision(*hit.getComponent<ColliderComponent>());
}


void ProjectileComponent::die() const {
	m_GameObject->setActive(false);
	Game::getInstance()->getCurrentScene()->m_ProjectilePool.freeObjectIntoPool(m_GameObject);
}

void ProjectileComponent::initialize(const glm::vec3 _position, const glm::vec3 _direction, const float _speed, const float _damage, const unsigned char _layer) {
	m_GameObject->getTransform().setLocalPosition(_position);
	m_GameObject->getTransform().setLocalScale(glm::vec3(0.2f));
	m_Direction = glm::normalize(_direction);
	m_MovementSpeed = _speed;
	m_Damage = _damage;

	/* Auto-Add Collider */
	m_CircleColliderComponent = m_GameObject->getComponent<CircleColliderComponent>();
	if (m_CircleColliderComponent == nullptr) {
		m_CircleColliderComponent = m_GameObject->addComponent<>(new CircleColliderComponent());
		m_CircleCollider = new CircleCollider(0.2f, glm::vec3(0, 0, 0));
		m_CircleCollider->initialize(std::shared_ptr<GameObject>(m_GameObject));
		m_CircleCollider->setCollisionLayer(_layer);
		m_CircleColliderComponent->initialize(*m_CircleCollider);
	}
	else {
		m_CircleCollider = &m_CircleColliderComponent->getCollider();
		m_CircleCollider->setCollisionLayer(_layer);
	}
	m_GameObject->setActive(true);
}

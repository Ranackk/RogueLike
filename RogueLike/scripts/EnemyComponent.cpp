#include "stdafx.h"
#include "EnemyComponent.h"
#include "Game.h"
#include "CircleColliderComponent.h"
#include "PhysicsEngine.h"
#include "HealthComponent.h"
#include <string>


EnemyComponent::EnemyComponent()
{
	m_CircleCollider = nullptr;
	m_Scene = nullptr;
	m_HealthComponent = nullptr;
	m_MovementSpeed = 5.0f + 1.2f;
}

void EnemyComponent::initialize(Scene* _scene, const glm::vec3 _position) {
	m_GameObject->getTransform().setLocalPosition(_position);
	m_GameObject->getTransform().setLocalScale(glm::vec3(0.5f, 1.0f, 0.5));
	m_Scene = _scene;

	/* Render Component */
	const GLuint texture = Game::getInstance()->getTextureManager()->getTextureByIdentifier("tex_Player");
	std::shared_ptr<Material> material = Game::getInstance()->getMaterialManager()->getMaterialByName("mat_Player");
	const std::shared_ptr<ModelData> modelData = Game::getInstance()->getModelManager()->getModelDataByIdentifier("mesh_Player");
	material->setTexture(texture);

	RenderComponent* rc = m_GameObject->addComponent<>(new RenderComponent());
	rc->initialize(modelData, material);

	/* Collider */
	CircleColliderComponent* cc = m_GameObject->addComponent<>(new CircleColliderComponent());
	m_CircleCollider = new CircleCollider(0.25f, glm::vec3(0, 0, 0));
	m_CircleCollider->initialize(std::shared_ptr<GameObject>(m_GameObject));
	cc->initialize(*m_CircleCollider);

	/* Add Health Component */
	m_HealthComponent = m_GameObject->addComponent(new HealthComponent());
	m_HealthComponent->initialize(2, 2);

	/* Basic AI */
	m_StartPosition = _position;
}

void EnemyComponent::update(GLFWwindow* window, const float deltaTime) {
	m_AliveTime += deltaTime;

	const glm::vec3 newPosition = m_StartPosition + 0.5f * glm::vec3(cos(m_AliveTime), 0.0f, sin(m_AliveTime));

	//m_GameObject->getTransform().setLocalPosition(newPosition);
	//std::cout << "Enemy: " << getGameObject()->getName().c_str() << "Pos: " << m_GameObject->getTransform().getLocalPosition().z << std::endl;
}

void EnemyComponent::takeDamage(const float _amount) {
	std::cout << m_GameObject->getName().c_str() << " took " << _amount << " damage! NEW HP: " << std::to_string((*m_HealthComponent->getCurrentHealthPointer())) << std::endl;
	if (m_HealthComponent->takeDamage(_amount)) {
		die();
	}
}

void EnemyComponent::die() {
	std::cout << "EnemyComponent would die now!" << std::endl;
}

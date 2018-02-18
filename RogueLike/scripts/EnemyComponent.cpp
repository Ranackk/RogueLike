#include "stdafx.h"
#include "EnemyComponent.h"
#include "Game.h"
#include "CircleColliderComponent.h"
#include "PhysicsEngine.h"


EnemyComponent::EnemyComponent()
{
	m_CircleCollider = nullptr;
}

void EnemyComponent::initialize(Scene* _scene, const glm::vec3 _position) {
	m_GameObject->getTransform().setLocalPosition(_position);
	m_GameObject->getTransform().setLocalScale(glm::vec3(0.5f, 1.0f, 0.5));
	m_MovementSpeed = 5.0f + 1.2f;
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
	m_CircleCollider = new CircleCollider(0.5f, glm::vec3(0, 0, 0));
	m_CircleCollider->initialize(std::shared_ptr<GameObject>(m_GameObject));
	cc->initialize(*m_CircleCollider);

	/* Basic AI */
	m_StartPosition = _position;
}

void EnemyComponent::update(GLFWwindow* window, const float deltaTime) {
	m_AliveTime += deltaTime;

	const glm::vec3 newPosition = m_StartPosition + 0.5f * glm::vec3(cos(m_AliveTime), 0.0f, sin(m_AliveTime));

	m_GameObject->getTransform().setLocalPosition(newPosition);
	//std::cout << "Enemy: " << getGameObject()->getName().c_str() << "Pos: " << m_GameObject->getTransform().getLocalPosition().z << std::endl;
}

void EnemyComponent::die() {
	m_Alive = false;
	std::cout << "EnemyComponent dead!" << std::endl;
}

bool EnemyComponent::isAlive() const {
	return m_Alive;
}

EnemyComponent::~EnemyComponent() {
	delete m_CircleCollider;
}

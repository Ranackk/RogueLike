#include "stdafx.h"
#include "Enemy.h"
#include "Game.h"
#include "CircleColliderComponent.h"
#include "PhysicsEngine.h"


Enemy::Enemy()
{
	m_CircleCollider = nullptr;
}

void Enemy::initialize(Scene* _scene, const glm::vec3 _position) {
	m_Transform.setLocalPosition(_position);
	m_Transform.setLocalScale(glm::vec3(0.2f));
	m_MovementSpeed = 5.0f + 1.2f;
	m_Scene = _scene;

	/* Render Component */
	const GLuint texture = Game::getInstance()->getTextureManager()->getTextureByIdentifier("tex_Enemy");

	std::shared_ptr<Material> material = Game::getInstance()->getMaterialManager()->getMaterialByName("mat_Enemy");
	material->setupBaseShader(glm::vec4(1, 1, 1, 1), texture, Game::getInstance()->getMaterialManager()->m_Skybox);

	const std::shared_ptr<ModelData> modelData = Game::getInstance()->getModelManager()->getModelDataByIdentifier("mesh_Anglerfish");

	RenderComponent* rc = addComponent<>(new RenderComponent());
	rc->initialize(modelData, material);

	/* Collider */
	CircleColliderComponent* cc = addComponent<>(new CircleColliderComponent());
	m_CircleCollider = new CircleCollider(0.5f, glm::vec3(0, 0, 0));
	m_CircleCollider->initialize(std::shared_ptr<Enemy>(this));
	cc->initialize(*m_CircleCollider);
}

void Enemy::update(GLFWwindow* window, const float deltaTime) {

}

void Enemy::die() {
	m_Alive = false;
	std::cout << "Enemy dead!" << std::endl;
}

bool Enemy::isAlive() const {
	return m_Alive;
}

Enemy::~Enemy() {
	delete m_CircleCollider;
}

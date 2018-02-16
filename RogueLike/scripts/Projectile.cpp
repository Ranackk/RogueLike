#include "stdafx.h"
#include "Projectile.h"
#include "Scene.h"
#include "Game.h"
#include "CircleColliderComponent.h"
#include "PhysicsEngine.h"


Projectile::Projectile()
{
}

void Projectile::update(GLFWwindow* window, const float deltaTime) {
	m_Transform.translate(m_Direction * m_MovementSpeed);

	// Collision with Map
	if (Game::getInstance()->getCurrentScene()->collidesWithSceneGeometry(*m_CircleCollider)) {
		std::cout << "Projectile hit map!" << std::endl;
	}

	// Collision with Enemies
	EnemyComponent& hit = EnemyComponent();
	if (Game::getInstance()->getCurrentScene()->collidesWithEnemies(*m_CircleCollider, hit)) {
		std::cout << "Projectile hit enemy!" << std::endl;
	}

	// Collision with PlayerComponent
	if (Game::getInstance()->getCurrentScene()->collidesWithPlayer(*m_CircleCollider)) {
		std::cout << "Projectile hit player!" << std::endl;
	}


	// Generic approach: Check for every collision, then send collision events such as:
	//hit.getComponent<ColliderComponent>()->fireCollision(*m_CircleColliderComponent);
	//m_CircleColliderComponent->fireCollision(*hit.getComponent<ColliderComponent>());
}


void Projectile::initialize(Scene* _scene, const glm::vec3 _position, const glm::vec3 _direction, const float _speed, const unsigned char _layer) {
	m_Transform.setLocalPosition(_position);
	m_Transform.setLocalScale(glm::vec3(0.2f));
	m_Direction = _direction;
	m_MovementSpeed = _speed;
	m_Scene = _scene;

	/* Render Component */
	const GLuint texture = Game::getInstance()->getTextureManager()->getTextureByIdentifier("tex_Player");

	std::shared_ptr<Material> material = Game::getInstance()->getMaterialManager()->getMaterialByName("mat_Player");
	material->setupBaseShader(glm::vec4(1, 1, 1, 1), texture, Game::getInstance()->getMaterialManager()->m_Skybox);

	const std::shared_ptr<ModelData> modelData = Game::getInstance()->getModelManager()->getModelDataByIdentifier("mesh_Player");

	RenderComponent* rc = addComponent<>(new RenderComponent());
	rc->initialize(modelData, material);

	/* Collider */
	m_CircleColliderComponent = addComponent<>(new CircleColliderComponent());
	m_CircleCollider = new CircleCollider(0.5f, glm::vec3(0, 0, 0));
	m_CircleCollider->initialize(std::shared_ptr<Projectile>(this));
	m_CircleCollider->setCollisionLayer(_layer);
	m_CircleColliderComponent->initialize(*m_CircleCollider);
}

#include "stdafx.h"
#include "FieldComponent.h"
#include "Game.h"
#include "ColliderComponent.h"
#include "BoxCollider.h"
#include "BoxColliderComponent.h"
#include "PhysicsEngine.h"

// This function does not yet create the field!
FieldComponent::FieldComponent() {
	m_FieldType = FieldType::FLOOR;
	m_Map = nullptr;
	//m_Material = nullptr;
}

void FieldComponent::initialize(Scene* map, const glm::vec2 worldGridPosition, const FieldType fieldType) {
	m_Map = map;
	m_fieldID = worldGridPosition.x + worldGridPosition.y * (map->getRoomsPerMap().y * Game::m_s_cRoomWidthInFields);
	m_worldGridPosition = worldGridPosition;
	m_FieldType = fieldType;

	m_GameObject->getTransform().setLocalPosition(glm::vec3(worldGridPosition.x, 0, worldGridPosition.y));

	/* Create Render Component */
	const GLuint texture = Game::getInstance()->getTextureManager()->getTextureByIdentifier(fieldType.getTextureIdentifier());

	std::shared_ptr<Material> material = Game::getInstance()->getMaterialManager()->getMaterialByName(fieldType.getMaterialIdentifer());
	material->setTexture(texture);

	const std::shared_ptr<ModelData> modelData = Game::getInstance()->getModelManager()->getModelDataByIdentifier(fieldType.getModelIdentifier());

	RenderComponent* rc = m_GameObject->addComponent(new RenderComponent());
	rc->initialize(modelData, material);

	if (m_FieldType.getPassing() == FieldType::BLOCKED) {
		BoxColliderComponent* cc = m_GameObject->addComponent(new BoxColliderComponent());
		BoxCollider bc = BoxCollider(glm::vec2(1, 1), glm::vec3(-0.5, 0, -0.5));
		bc.initialize(std::shared_ptr<GameObject>(m_GameObject));
		bc.setCollisionLayer(CollisionLayer::MAP_GEOMETRY);
		cc->initialize(bc);
	}
}
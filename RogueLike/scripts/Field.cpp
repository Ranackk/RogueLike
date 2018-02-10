#include "stdafx.h"
#include "Field.h"
#include "Game.h"
#include "ColliderComponent.h"
#include "BoxCollider.h"
#include "BoxColliderComponent.h"

// This function does not yet create the field!
Field::Field() : GameObject() {
	m_FieldType = FieldType::FLOOR;
	m_Map = nullptr;
	//m_Material = nullptr;
}

void Field::initialize(Scene* map, const glm::vec2 worldGridPosition, const FieldType fieldType) {
	m_Map = map;
	m_fieldID = worldGridPosition.x + worldGridPosition.y * (map->getRoomsPerMap().y * Game::m_s_cRoomWidthInFields);
	m_worldGridPosition = worldGridPosition;
	m_FieldType = fieldType;

	m_Transform.setLocalPosition(glm::vec3(worldGridPosition.x, 0, worldGridPosition.y));

	/* Create Render Component */
	const GLuint texture = Game::getInstance()->getTextureManager()->getTextureByIdentifier(fieldType.getTextureIdentifier());

	std::shared_ptr<Material> material = Game::getInstance()->getMaterialManager()->getMaterialByName(fieldType.getMaterialIdentifer());
	material->setupBaseShader(glm::vec4(1, 1, 1, 1), texture, Game::getInstance()->getMaterialManager()->m_Skybox);

	const std::shared_ptr<ModelData> modelData = Game::getInstance()->getModelManager()->getModelDataByIdentifier(fieldType.getModelIdentifier());

	RenderComponent* rc = addComponent(new RenderComponent());
	rc->initialize(modelData, material);

	if (m_FieldType.getPassing() == FieldType::BLOCKED) {
		BoxColliderComponent* cc = addComponent(new BoxColliderComponent());
		BoxCollider bc = BoxCollider(glm::vec2(1, 1), glm::vec3(-0.5, 0, -0.5));
		bc.initialize(std::shared_ptr<Field>(this));
		cc->initialize(bc);
	}
}

//Field::Field(Scene *map, const glm::vec2 worldGridPosition, const FieldType fieldType) {
//	m_Map = map;
//	m_fieldID = worldGridPosition.x + worldGridPosition.y * (map->getRoomsPerMap().y * Game::m_s_cRoomWidthInFields);
//	m_worldGridPosition = worldGridPosition;
//	m_FieldType = fieldType;
//	 
//	m_Transform.setLocalPosition(glm::vec3(worldGridPosition.x, 0, worldGridPosition.y));
//
//	m_Material = Game::getInstance()->getMaterialManager()->getMaterialByName(fieldType.getTextureIdentifier());
//	m_Material->setupBaseShader(glm::vec4(1, 1, 1, 1), Game::getInstance()->getTextureManager()->getTextureByIdentifier(fieldType.getTextureIdentifier()), Game::getInstance()->getMaterialManager()->m_Skybox);
//
//}

//void Field::initializeModel() {
//	initialize(m_FieldType.getModelIdentifier());
//}
//
//Field::~Field()
//{
//}
//
//void Field::update(GLFWwindow* window, const float deltaTime) {}
//

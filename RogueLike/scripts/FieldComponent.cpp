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

void FieldComponent::setRoomCoord(const glm::vec2 _roomCoord)
{
	m_RoomCoord = _roomCoord;

}

void FieldComponent::update(GLFWwindow* window, const float deltaTime) {
	if (!m_FirstUpdateHappened) {
		m_FirstUpdateHappened = true;
		if (m_FieldType == FieldType::DOOR_NORTH || m_FieldType == FieldType::DOOR_EAST || m_FieldType == FieldType::DOOR_SOUTH || m_FieldType == FieldType::DOOR_WEST) {
			m_isDoor = true;
		}
	}
	if (m_isDoor) {
		// Is player in this room
		if (m_Map->isPlayerFullyInRoom(m_RoomCoord)) {
			// Are there any enemies?
			if (m_Map->isEnemyInRoomCoord(m_RoomCoord)) {

				if (!m_CurrentlyLocked) {
					m_CurrentlyLocked = true;
					m_Map->blockDoor(m_FieldType, m_worldGridPosition);
				}
				// If yes, set collider active & perform some kind of visual effect via material
				//std::cout << m_GameObject->getName().c_str() << " yells " << "BLOCKADE!" << std::endl;
				
			}
			else {
				if (m_CurrentlyLocked) {
					m_CurrentlyLocked = false;
					m_Map->unblockDoor(m_FieldType);
				}
			}
		}
	}
}

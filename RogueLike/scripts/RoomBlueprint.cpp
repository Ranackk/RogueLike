#include "stdafx.h"
#include "RoomBlueprint.h"
#include "FieldType.h"


RoomBlueprint::RoomBlueprint()
{
	m_fieldData = nullptr;
}


RoomBlueprint::~RoomBlueprint()
{
}

void RoomBlueprint::fillWithVoid(const glm::vec2 _roomSize) {
	this->m_fieldData = new FieldType[(int) (_roomSize.x * _roomSize.y)];
	this->m_roomSize = _roomSize;

	for (int iX = 0; iX < _roomSize.x; iX++) {
		for (int iY = 0; iY < _roomSize.y; iY++) {
			this->m_fieldData[static_cast<int>(iX + iY * _roomSize.x)] = FieldType::VOID;
		}
	}
}

void RoomBlueprint::fillWithTypes(const glm::vec2 _roomSize, FieldType * _fillData)
{
	this->m_fieldData = new FieldType[(int) (_roomSize.x * _roomSize.y)];
	this->m_fieldData = _fillData;
	this->m_roomSize = _roomSize;

	/* If room is empty, fill it with empty */
	if (this->m_fieldData == nullptr) {
		for (int iX = 0; iX < _roomSize.x; iX++) {
			for (int iY = 0; iY < _roomSize.y; iY++) {
				this->m_fieldData[static_cast<int>(iX + iY * _roomSize.x)] = FieldType::FLOOR;
			}
		}
	}

	/* CHeck for doors */
	for (int iX = 0; iX < m_roomSize.x; iX++) {
		for (int iY = 0; iY < m_roomSize.y; iY++) {
			const FieldType ft = this->m_fieldData[static_cast<int>(iX + iY * m_roomSize.x)];
			if (ft == FieldType::DOOR_NORTH) m_HasLeftoverDoorFacingNorth = true;
			else if (ft == FieldType::DOOR_EAST) m_HasLeftoverDoorFacingEast = true;
			else if (ft == FieldType::DOOR_SOUTH) m_HasLeftoverDoorFacingSouth = true;
			else if (ft == FieldType::DOOR_WEST) m_HasLeftoverDoorFacingWest = true;
		}
	}
}

//
//void RoomBlueprint::fillLightAndEnemyVector(const std::vector<glm::vec2> _lightPositions,
//	const std::vector<std::pair<glm::vec2, EnemyComponent::EnemyType>> _enemyInformation) {
//	this->m_LightPositions = _lightPositions;
//	this->m_EnemyInformation = _enemyInformation;
//}

void RoomBlueprint::fillWithBlueprint(RoomBlueprint* _data) {
	this->m_roomSize = _data->m_roomSize;
	this->m_fieldData = new FieldType[(int) (m_roomSize.x * m_roomSize.y)];
	this->m_fieldData = _data->getFieldData();

	/* If room is empty, fill it with empty */
	if (this->m_fieldData == nullptr) {
		for (int iX = 0; iX < m_roomSize.x; iX++) {
			for (int iY = 0; iY < m_roomSize.y; iY++) {
				this->m_fieldData[static_cast<int>(iX + iY * m_roomSize.x)] = FieldType::FLOOR;
			}
		}
	}

	/* CHeck for doors */
	for (int iX = 0; iX < m_roomSize.x; iX++) {
		for (int iY = 0; iY < m_roomSize.y; iY++) {
			const FieldType ft = this->m_fieldData[static_cast<int>(iX + iY * m_roomSize.x)];
			if (ft == FieldType::DOOR_NORTH) m_HasLeftoverDoorFacingNorth = true;
			else if (ft == FieldType::DOOR_EAST) m_HasLeftoverDoorFacingEast = true;
			else if (ft == FieldType::DOOR_SOUTH) m_HasLeftoverDoorFacingSouth = true;
			else if (ft == FieldType::DOOR_WEST) m_HasLeftoverDoorFacingWest = true;
		}
	}
	this->m_LightPositions = _data->m_LightPositions;
	this->m_EnemyInformation = _data->m_EnemyInformation;

	this->m_PlacedInWorld = true;
}

FieldType RoomBlueprint::getFieldTypeAt(const glm::vec2 _relativePosition) const {
	return m_fieldData[static_cast<int>(_relativePosition.y * m_roomSize.x + _relativePosition.x)];
}

FieldType* RoomBlueprint::getFieldData() const {
	return m_fieldData;
}

std::vector<int> RoomBlueprint::getLeftoverDoorsIndices() const
{
	 std::vector<int> ret = std::vector<int>();
	 if (m_HasLeftoverDoorFacingNorth) ret.push_back(0);
	 if (m_HasLeftoverDoorFacingEast) ret.push_back(1);
	 if (m_HasLeftoverDoorFacingSouth) ret.push_back(2);
	 if (m_HasLeftoverDoorFacingWest) ret.push_back(3);
	 return ret;
}

void RoomBlueprint::markDoorAsUsed(const int direction)
{
	switch (direction) {
	case 0: m_HasLeftoverDoorFacingNorth = false; break;
	case 1: m_HasLeftoverDoorFacingEast = false; break;
	case 2: m_HasLeftoverDoorFacingSouth = false; break;
	case 3: m_HasLeftoverDoorFacingWest = false; break;
	default: break;
	}
}

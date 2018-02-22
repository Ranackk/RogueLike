#pragma once
#include "EnemyComponent.h"
#include "LightComponent.h"
class FieldType;

class RoomBlueprint
{
public:
	RoomBlueprint();
	~RoomBlueprint();

	void fillWithTypes(const glm::vec2 _roomSize, FieldType* _fieldData);
	void fillLightAndEnemyVector(const std::vector<glm::vec2> _lightPositions, const std::vector<std::pair<glm::vec2, EnemyComponent::EnemyType>> _enemyInformation);
	FieldType getFieldTypeAt(const glm::vec2 _relativePosition) const;
	FieldType* getFieldData() const;

	std::vector<std::pair<glm::vec2, EnemyComponent::EnemyType>> m_EnemyInformation;
	std::vector<glm::vec2> m_LightPositions;

	bool m_HasDoorFacingNorth = false;
	bool m_HasDoorFacingEast = false;
	bool m_HasDoorFacingSouth = false;
	bool m_HasDoorFacingWest = false;

private:
	glm::vec2 m_roomSize;
	FieldType* m_fieldData;

};


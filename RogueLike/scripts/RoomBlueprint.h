#pragma once
#include "Enemy.h"
#include "Light.h"
class FieldType;

class RoomBlueprint
{
public:
	RoomBlueprint();
	~RoomBlueprint();

	void fillWithTypes(const glm::vec2 _roomSize, FieldType* _fieldData);
	void fillLightAndEnemyVector(const std::vector<glm::vec2> _lightPositions, const std::vector<glm::vec2> _enemyPositions);
	FieldType getFieldTypeAt(const glm::vec2 _relativePosition) const;
	FieldType* getFieldData() const;

	std::vector<glm::vec2> m_EnemyPositions;
	std::vector<glm::vec2> m_LightPositions;

private:
	glm::vec2 m_roomSize;
	FieldType* m_fieldData;
};


#pragma once
#include "EnemyComponent.h"
#include "LightComponent.h"
class FieldType;

class RoomBlueprint
{
public:
	RoomBlueprint();
	~RoomBlueprint();

	void fillWithVoid(const glm::vec2 _roomSize);
	void fillWithTypes(const glm::vec2 _roomSize, FieldType* _fieldData);
	//void fillLightAndEnemyVector(const std::vector<glm::vec2> _lightPositions, const std::vector<std::pair<glm::vec2, EnemyComponent::EnemyType>> _enemyInformation);

	void fillWithBlueprint(RoomBlueprint* _data);

	FieldType getFieldTypeAt(const glm::vec2 _relativePosition) const;
	FieldType* getFieldData() const;

	std::vector<std::pair<glm::vec2, EnemyComponent::EnemyType>> m_EnemyInformation;
	std::vector<glm::vec3> m_LightPositions;

	std::vector<int> getLeftoverDoorsIndices() const;

	void markDoorAsUsed(int direction);
	bool isPlacedInWorld() const { return m_PlacedInWorld; }

	bool m_HasLeftoverDoorFacingNorth = false;
	bool m_HasLeftoverDoorFacingEast = false;
	bool m_HasLeftoverDoorFacingSouth = false;
	bool m_HasLeftoverDoorFacingWest = false;
private:
	bool m_PlacedInWorld = false;
	glm::vec2 m_roomSize;
	FieldType* m_fieldData;


};


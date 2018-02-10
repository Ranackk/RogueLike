#pragma once
#include "FieldType.h"
#include "GameObject.h"
class Scene;

class Field : public GameObject
{
public:
	Field();
	void initialize(Scene *map, const glm::vec2 worldGridPosition, const FieldType fieldType);

	void update(GLFWwindow* window, const float deltaTime) override {}

	FieldType m_FieldType;
private:
	int m_roomID = -1;			// TODO: Add roomIDs
	int m_fieldID = -1;
	glm::vec2 m_worldGridPosition;
	Scene* m_Map;

}; 


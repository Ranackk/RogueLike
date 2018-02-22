#pragma once
#include "FieldType.h"
#include "GameObject.h"
class Scene;

class FieldComponent : public UpdateComponent
{
public:
	FieldComponent();
	void initialize(Scene *map, const glm::vec2 worldGridPosition, const FieldType fieldType);
	void setRoomCoord(glm::vec2 _roomCoord);
	void update(GLFWwindow* window, const float deltaTime) override {}

	FieldType m_FieldType;
private:
	glm::vec2 m_RoomCoord;
	int m_fieldID = -1;
	glm::vec2 m_worldGridPosition;
	Scene* m_Map;

}; 


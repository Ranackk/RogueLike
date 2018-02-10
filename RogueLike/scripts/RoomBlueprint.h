#pragma once
class FieldType;

class RoomBlueprint
{
public:
	RoomBlueprint();
	~RoomBlueprint();

	void fillWithTypes(const glm::vec2 _roomSize, FieldType* _fieldData);
	FieldType getFieldTypeAt(const glm::vec2 _relativePosition) const;
	FieldType* getFieldData() const;

private:
	glm::vec2 m_roomSize;
	FieldType* m_fieldData;
};


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

void RoomBlueprint::fillWithTypes(const glm::vec2 _roomSize, FieldType * _fillData)
{
	this->m_fieldData = new FieldType[_roomSize.x * _roomSize.y];
	this->m_fieldData = _fillData;
	this->m_roomSize = _roomSize;

	/* If room is empty, fill it with empty */
	if (this->m_fieldData == nullptr) {
		for (int iX = 0; iX < _roomSize.x; iX++) {
			for (int iY = 0; iY < _roomSize.y; iY++) {
				this->m_fieldData[(int)(iX + iY * _roomSize.x)] = FieldType::FLOOR;
			}
		}
	}
}

FieldType RoomBlueprint::getFieldTypeAt(const glm::vec2 _relativePosition) const {
	return m_fieldData[(int)(_relativePosition.y * m_roomSize.x + _relativePosition.x)];
}

FieldType* RoomBlueprint::getFieldData() const {
	return m_fieldData;
}

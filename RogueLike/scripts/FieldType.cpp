#include "stdafx.h"
#include "FieldType.h"
#include "Game.h"

FieldType FieldType::FLOOR = FieldType("floor", "baseFloor", WALKING);
FieldType FieldType::WALL = FieldType("wall", "baseWall", BLOCKED);


FieldType::FieldType() {
	m_textureHandle = "";
	m_modelHandle = "";
	m_walkability = WALKING;
}

FieldType::~FieldType() {
}

FieldType::FieldType(const std::string textureHandle, const std::string modelHandle, const Walkability passing) {
	m_textureHandle = textureHandle;
	m_modelHandle = modelHandle;
	m_walkability = passing;
}

FieldType FieldType::byColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	//std::cout << "Trying to interpret color: " << (int)r << ", " << (int)g << ", " << (int)b << ", " << (int)a << std::endl;
	if ((int)r == 81 && (int)g == 60 && (int)b == 119) {
		return FLOOR;
	}
	return WALL;
}

std::string FieldType::toString() const{
	return m_textureHandle;
}

GLuint FieldType::getSprite() const {
	return Game::getInstance()->getTextureManager()->getTextureByIdentifier(m_textureHandle);
}

std::string FieldType::getTextureIdentifier() const {
	return m_textureHandle;
}

std::string FieldType::getModelIdentifier() const {
	return m_modelHandle;
}
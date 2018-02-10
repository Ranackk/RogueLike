#include "stdafx.h"
#include "FieldType.h"
#include "Game.h"

FieldType FieldType::FLOOR = FieldType("tex_Floor", "mat_Floor", "mesh_Floor", WALKING);
FieldType FieldType::WALL = FieldType("tex_Wall", "mat_Wall", "mesh_Wall", BLOCKED);


FieldType::FieldType() {
	m_textureHandle = "";
	m_modelHandle = "";
	m_walkability = WALKING;
}

FieldType::~FieldType() {
}

FieldType::FieldType(const std::string textureHandle, const std::string materialHandle, const std::string modelHandle, const Walkability passing) {
	m_textureHandle = textureHandle;
	m_modelHandle = modelHandle;
	m_materialHandle = materialHandle;
	m_walkability = passing;
}

FieldType FieldType::byColor(const unsigned char r) {
	if (static_cast<int>(r) == 50) {
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

std::string FieldType::getMaterialIdentifer() const {
	return m_materialHandle;
}

std::string FieldType::getModelIdentifier() const {
	return m_modelHandle;
}
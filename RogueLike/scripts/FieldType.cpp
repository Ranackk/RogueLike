#include "stdafx.h"
#include "FieldType.h"
#include "Game.h"

FieldType FieldType::FLOOR = FieldType(0, "tex_Floor", "mat_Floor", "mesh_Floor", WALKING);
FieldType FieldType::WALL_X = FieldType(1, "tex_Wall", "mat_Wall", "mesh_Wall_basic_x", BLOCKED);
FieldType FieldType::WALL_Z = FieldType(2, "tex_Wall", "mat_Wall", "mesh_Wall_basic_z", BLOCKED);

FieldType FieldType::WALL_ARCH_L_X = FieldType(3, "tex_Wall", "mat_Wall", "mesh_Wall_arch_left_x", BLOCKED);
FieldType FieldType::WALL_ARCH_L_Z = FieldType(4, "tex_Wall", "mat_Wall", "mesh_Wall_arch_left_z", BLOCKED);
FieldType FieldType::WALL_ARCH_R_X = FieldType(5, "tex_Wall", "mat_Wall", "mesh_Wall_arch_right_x", BLOCKED);
FieldType FieldType::WALL_ARCH_R_Z = FieldType(6, "tex_Wall", "mat_Wall", "mesh_Wall_arch_right_z", BLOCKED);

FieldType FieldType::WALL_CORNER_NW = FieldType(7, "tex_Wall", "mat_Wall", "mesh_Wall_corner_left_back", BLOCKED);
FieldType FieldType::WALL_CORNER_NE = FieldType(8, "tex_Wall", "mat_Wall", "mesh_Wall_corner_right_back", BLOCKED);
FieldType FieldType::WALL_CORNER_SW = FieldType(9, "tex_Wall", "mat_Wall", "mesh_Wall_corner_left_front", BLOCKED);
FieldType FieldType::WALL_CORNER_SE = FieldType(10, "tex_Wall", "mat_Wall", "mesh_Wall_corner_right_front", BLOCKED);


FieldType::FieldType() {
	m_textureHandle = "";
	m_modelHandle = "";
	m_walkability = WALKING;
	m_Id = -1;
}

FieldType::~FieldType() {
}

FieldType::FieldType(const int id, const std::string textureHandle, const std::string materialHandle, const std::string modelHandle, const Walkability passing) {
	m_textureHandle = textureHandle;
	m_modelHandle = modelHandle;
	m_materialHandle = materialHandle;
	m_walkability = passing;
	m_Id = id;
}

FieldType FieldType::byColor(const unsigned char r) {
	const int red = static_cast<int>(r);
	switch (red) {
	case 0: return WALL_X;
	case 10: return WALL_Z;

	case 20: return WALL_ARCH_L_X;
	case 30: return WALL_ARCH_L_Z;

	case 40: return WALL_ARCH_R_X;
	case 50: return WALL_ARCH_R_Z;

	case 60: return WALL_CORNER_NW;
	case 70: return WALL_CORNER_NE;
	case 80: return WALL_CORNER_SW;
	case 90: return WALL_CORNER_SE;

	case 255: return FLOOR;

	default: return FLOOR;
	}
	return WALL_X;
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
#include "stdafx.h"
#include "FieldType.h"
#include "Game.h"

FieldType FieldType::VOID = FieldType(-1, "tex_Floor", "mat_Floor", "mesh_Floor", BLOCKED);

FieldType FieldType::FLOOR = FieldType(0, "tex_Floor", "mat_Floor", "mesh_Floor", WALKING);

FieldType FieldType::DOOR_NORTH = FieldType(1, "tex_Floor", "mat_Floor", "mesh_Floor", WALKING);
FieldType FieldType::DOOR_EAST  = FieldType(2, "tex_Floor", "mat_Floor", "mesh_Floor", WALKING);
FieldType FieldType::DOOR_SOUTH = FieldType(3, "tex_Floor", "mat_Floor", "mesh_Floor", WALKING);
FieldType FieldType::DOOR_WEST  = FieldType(4, "tex_Floor", "mat_Floor", "mesh_Floor", WALKING);

FieldType FieldType::WALL_X = FieldType(5, "tex_Wall", "mat_Wall", "mesh_Wall_basic_x", BLOCKED);
FieldType FieldType::WALL_Z = FieldType(6, "tex_Wall", "mat_Wall", "mesh_Wall_basic_z", BLOCKED);

FieldType FieldType::WALL_ARCH_L_X = FieldType(7, "tex_Wall", "mat_Wall", "mesh_Wall_arch_left_x", BLOCKED);
FieldType FieldType::WALL_ARCH_L_Z = FieldType(8, "tex_Wall", "mat_Wall", "mesh_Wall_arch_left_z", BLOCKED);
FieldType FieldType::WALL_ARCH_R_X = FieldType(9, "tex_Wall", "mat_Wall", "mesh_Wall_arch_right_x", BLOCKED);
FieldType FieldType::WALL_ARCH_R_Z = FieldType(10, "tex_Wall", "mat_Wall", "mesh_Wall_arch_right_z", BLOCKED);

FieldType FieldType::WALL_CORNER_NW = FieldType(11, "tex_Wall", "mat_Wall", "mesh_Wall_corner_left_back", BLOCKED);
FieldType FieldType::WALL_CORNER_NE = FieldType(12, "tex_Wall", "mat_Wall", "mesh_Wall_corner_right_back", BLOCKED);
FieldType FieldType::WALL_CORNER_SW = FieldType(13, "tex_Wall", "mat_Wall", "mesh_Wall_corner_left_front", BLOCKED);
FieldType FieldType::WALL_CORNER_SE = FieldType(14, "tex_Wall", "mat_Wall", "mesh_Wall_corner_right_front", BLOCKED);

FieldType FieldType::WALL_COLUMN_POS_X = FieldType(15, "tex_Wall", "mat_Wall", "mesh_Wall_column_pos_x", BLOCKED);
FieldType FieldType::WALL_COLUMN_NEG_X = FieldType(16, "tex_Wall", "mat_Wall", "mesh_Wall_column_neg_x", BLOCKED);
FieldType FieldType::WALL_COLUMN_POS_Z = FieldType(17, "tex_Wall", "mat_Wall", "mesh_Wall_column_pos_z", BLOCKED);
FieldType FieldType::WALL_COLUMN_NEG_Z = FieldType(18, "tex_Wall", "mat_Wall", "mesh_Wall_column_neg_z", BLOCKED);

FieldType FieldType::WALL_WINDOW_X = FieldType(19, "tex_Wall", "mat_Wall", "mesh_Wall_window_x", BLOCKED);
FieldType FieldType::WALL_WINDOW_Z = FieldType(20, "tex_Wall", "mat_Wall", "mesh_Wall_window_x", BLOCKED);

FieldType FieldType::WALL_BROKEN_X = FieldType(21, "tex_Wall", "mat_Wall", "mesh_Wall_broken_z", BLOCKED);
FieldType FieldType::WALL_BROKEN_Z = FieldType(22, "tex_Wall", "mat_Wall", "mesh_Wall_broken_z", BLOCKED);

FieldType FieldType::SHRINE = FieldType(100, "tex_Wall", "mat_Floor", "mesh_Shrine", BLOCKED);

FieldType FieldType::ROCKS = FieldType(100, "tex_Wall", "mat_Floor", "mesh_Rocks", BLOCKED);

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

	case 100: return WALL_COLUMN_POS_X;
	case 110: return WALL_COLUMN_NEG_X;
	case 120: return WALL_COLUMN_POS_Z;
	case 130: return WALL_COLUMN_NEG_Z;

	case 140: return WALL_WINDOW_X;
	case 150: return WALL_WINDOW_Z;

	case 160: return WALL_BROKEN_X;
	case 170: return WALL_BROKEN_Z;

	case 180: return SHRINE;

	case 190: return ROCKS;

	case 200: 
		return VOID;

	case 210: return DOOR_NORTH;
	case 220: return DOOR_EAST;
	case 230: return DOOR_SOUTH;
	case 240: return DOOR_WEST;

	case 255: return FLOOR;

	default: return VOID;
	}
}

bool FieldType::isVoid() const {
	return m_Id == -1;
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
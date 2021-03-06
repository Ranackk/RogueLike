#pragma once
#include "stdafx.h"

class FieldType
{
public:
	enum Walkability {
		WALKING, FLYING, BLOCKED
	};

	static FieldType VOID;

	static FieldType FLOOR;

	static FieldType DOOR_NORTH;
	static FieldType DOOR_EAST;
	static FieldType DOOR_SOUTH;
	static FieldType DOOR_WEST;

	static FieldType WALL_X;
	static FieldType WALL_Z;

	static FieldType WALL_ARCH_L_X;
	static FieldType WALL_ARCH_L_Z;
	static FieldType WALL_ARCH_R_X;
	static FieldType WALL_ARCH_R_Z;

	static FieldType WALL_CORNER_NW;
	static FieldType WALL_CORNER_NE;
	static FieldType WALL_CORNER_SW;
	static FieldType WALL_CORNER_SE;

	static FieldType WALL_COLUMN_POS_X;
	static FieldType WALL_COLUMN_NEG_X;
	static FieldType WALL_COLUMN_POS_Z;
	static FieldType WALL_COLUMN_NEG_Z;

	static FieldType WALL_WINDOW_X;
	static FieldType WALL_WINDOW_Z;

	static FieldType WALL_BROKEN_X;
	static FieldType WALL_BROKEN_Z;

	static FieldType ROCKS;

	static FieldType SHRINE;

	static FieldType WATER;

	static FieldType byColor(unsigned char r);
	bool isVoid() const;

	std::string toString() const;
	GLuint getSprite() const;
	std::string getModelIdentifier() const;
	std::string getTextureIdentifier() const;
	std::string getMaterialIdentifer() const;

	Walkability getPassing() const { return m_walkability; }
	int getDoorDirection() const { return m_Id - 1; }

	FieldType();
	~FieldType();

	bool operator <(const FieldType& rhs) const
	{
		return toString() < rhs.toString();
	}

	bool operator ==(const FieldType& rhs) const {
		return m_Id == rhs.m_Id;
	}

	int m_Id;
private:
	explicit FieldType(const int id, const std::string textureHandle, const std::string materialHandle, const std::string modelHandle, const Walkability walkability);

	Walkability m_walkability;
	std::string m_modelHandle;
	std::string m_textureHandle;
	std::string m_materialHandle;
};

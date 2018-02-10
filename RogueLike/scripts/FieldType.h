#pragma once
#include "stdafx.h"

class FieldType
{
public:
	enum Walkability {
		WALKING, BLOCKED
	};

	static FieldType FLOOR;
	static FieldType WALL;

	static FieldType byColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

	std::string toString() const;
	GLuint getSprite() const;
	std::string getModelIdentifier() const;
	std::string getTextureIdentifier() const;

	Walkability getPassing() const { return m_walkability; }

	FieldType();
	~FieldType();

	bool operator <(const FieldType& rhs) const
	{
		return toString() < rhs.toString();
	}
private:
	explicit FieldType(const std::string textureHandle, const std::string modelHandle, const Walkability walkability);

	Walkability m_walkability;
	std::string m_modelHandle;
	std::string m_textureHandle;
};

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

	static FieldType byColor(unsigned char r);

	std::string toString() const;
	GLuint getSprite() const;
	std::string getModelIdentifier() const;
	std::string getTextureIdentifier() const;
	std::string getMaterialIdentifer() const;

	Walkability getPassing() const { return m_walkability; }

	FieldType();
	~FieldType();

	bool operator <(const FieldType& rhs) const
	{
		return toString() < rhs.toString();
	}
private:
	explicit FieldType(const std::string textureHandle, const std::string materialHandle, const std::string modelHandle, const Walkability walkability);

	Walkability m_walkability;
	std::string m_modelHandle;
	std::string m_textureHandle;
	std::string m_materialHandle;
};

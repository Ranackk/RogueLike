#pragma once
#include "stdafx.h"

class CameraComponent;
class GameObject;
class Material;

enum Primitives {
	QUAD
};

class ModelData
{
public:

	// TODO: Primitve init via code (get modeldata by primitive type, use static map to modelmanager!

	int verts;

	//GLfloat *vertexBufferData; 
	// Still havent found a solution for storing an class wide array of declaration time unknown size
	std::vector<unsigned short> m_IndicesBufferData;
	std::vector<glm::vec3> m_VertexBufferData;
	std::vector<glm::vec2> m_UvBufferData;
	std::vector<glm::vec3> m_NormalBufferData;

	GLuint m_IndicesBufferID;
	GLuint m_VertexPositionBufferID;
	GLuint m_VertexUVBufferID;
	GLuint m_VertexNormalBufferID;

	//GLuint vertexColorBufferID;

	ModelData();
	//void initialize(const std::string modelPath);
	void initialize(std::vector<unsigned short> _indexBufferData, std::vector<glm::vec3> _vertexBufferData, std::vector<glm::vec2> _uvBufferData, std::vector<glm::vec3> _normalsBufferData);
	void draw() const;
	~ModelData();
};

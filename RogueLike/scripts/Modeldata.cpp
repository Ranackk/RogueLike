#include "stdafx.h"

#include "ModelData.h"
#include "GameObject.h"
#include "Utility.h"
#include "Camera.h"
#include "Material.h"

/*
* Part of a rendererable GameObject that stores all openGL buffer IDs needed to redner the object
*/
ModelData::ModelData()
{
	this->verts = 0;
	this->m_VertexPositionBufferID = 0;
	this->m_VertexNormalBufferID = 0;
	this->m_VertexUVBufferID = 0;
	this->m_IndicesBufferID = 0;
}

void ModelData::initialize(const std::string modelPath) {
	/* Load obj data into temporary containers */
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	const bool result = Utility::loadObj(std::string(modelPath).c_str(), vertices, uvs, normals);
	if (!result) {
		std::cout << "Error getting the modelinformation " << std::endl;
		return;
	}

	/* Index the loaded obj data into the correct buffers */
	Utility::indexVbo(vertices, uvs, normals, this->m_IndicesBufferData, this->m_VertexBufferData, this->m_UvBufferData, this->m_NormalBufferData);
	this->verts = this->m_VertexBufferData.size();

	// Generate the vertex buffer
	glGenBuffers(1, &this->m_VertexPositionBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_VertexPositionBufferID);
	// breakdown of this line: Use as <array buffer> of size <size>, starting at address <start ptr>, use <use method>
	glBufferData(GL_ARRAY_BUFFER, verts * sizeof(glm::vec3), &this->m_VertexBufferData[0], GL_STATIC_DRAW);

	// Generate the uv buffer
	glGenBuffers(1, &this->m_VertexUVBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_VertexUVBufferID);
	glBufferData(GL_ARRAY_BUFFER, verts * sizeof(glm::vec2), &this->m_UvBufferData[0], GL_STATIC_DRAW);

	// Generate the normal buffer
	glGenBuffers(1, &this->m_VertexNormalBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_VertexNormalBufferID);
	glBufferData(GL_ARRAY_BUFFER, verts * sizeof(glm::vec3), &this->m_NormalBufferData[0], GL_STATIC_DRAW);

	// Generate the indice buffer
	glGenBuffers(1, &this->m_IndicesBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_IndicesBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_IndicesBufferData.size() * sizeof(unsigned short), &this->m_IndicesBufferData[0], GL_STATIC_DRAW);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	printf("... Initialized ModelData \n"); 
}

void ModelData::draw() const
{
	// Drawing without VBO Indexing: glDrawArrays(GL_TRIANGLES, 0, verts);
	/* Bind the index Buffer */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_IndicesBufferID);
	/* Draw the index Buffer */
	glDrawElements(GL_TRIANGLES, this->m_IndicesBufferData.size(), GL_UNSIGNED_SHORT, (void*)0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


ModelData::~ModelData()
{
	//delete this->material;
}
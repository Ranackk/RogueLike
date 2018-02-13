#pragma once
#include "stdafx.h"
#include <vector>

class Utility
{
public:
	static bool loadShader(const std::string _shaderName, GLuint &_outId);
	static bool getQuadPrimitiveData(std::vector<glm::vec3>& _vertexBufferData, std::vector<glm::vec2>& _uvBufferData, std::vector<glm::vec3>& _normalsBufferData, std::vector<unsigned short>& _indexBufferData);
	static bool loadObj(const char *path, std::vector<glm::vec3> &outVertices, std::vector<glm::vec2> &outUVs, std::vector<glm::vec3> &outNormals);
	static bool indexVbo(std::vector<glm::vec3> &_inVertices, std::vector<glm::vec2> &_inUVs, std::vector<glm::vec3> &_inNormals,
		std::vector<unsigned short> & _outIndices, std::vector<glm::vec3> & _outVertices, std::vector<glm::vec2> & _outUvs, std::vector<glm::vec3> & _outNormals);
	static glm::mat4x4 translationMatrix(const glm::vec3 position);
	static glm::mat4x4 scaleMatrix(const glm::vec3 scale);
private:
	Utility() {};
	~Utility() {};
};


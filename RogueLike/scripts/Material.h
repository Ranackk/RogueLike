#pragma once
#include "stdafx.h"
#include <map>
#include "Camera.h"

class Material
{
public:
	enum Type {
		MATRICES_ONLY, BASE_SHADER
	};

	static void compileShader(std::string path);

	//virtual Material(std::string shaderName, const glm::vec4 _diffuseColor, const GLuint _textureID);
	Material(std::string shaderName);
	void setupMatricesOnly();
	void setupBaseShader(glm::vec4 _diffuseColor, GLuint _textureID, GLuint _skyboxID);

	~Material();

	//void bindMaterialForBatch(const Camera* _camera, const GLuint _matrixBufferID, const GLuint _vertexBufferID,
	//	const GLuint _uvBufferID, const GLuint _normalBufferID) const;
	void bindMaterial(glm::mat4x4 _perspectiveMatrix, glm::mat4x4 _viewMatrix, glm::mat4x4 _modelMatrix,
		const GLuint _vertexBufferID, const GLuint _uvBufferID, const GLuint _normalBufferID, const bool _instanced) const;
	void unbindMaterial();
	GLuint getShaderProgramId() const;

private:
	static std::map<std::string, GLuint> compiledShaders;
	Type m_Type;

	GLuint shaderProgramID;

	/* Matrix, Light Uniform GLuints */
	GLuint uniformModelMatrix;
	GLuint uniformViewMatrix;
	GLuint uniformProjectionMatrix;

	/* === Properties "BaseShader" === */
	/* Material Properties */
	glm::vec4 p_diffuseColor;
	GLuint p_textureID;

	/* Material Uniform GLuints */
	GLuint uniformDiffuseColor;
	GLuint uniformTexture;

	GLuint p_skyboxID;
	GLuint uniformSkybox;
	GLint uniformClippingPlane;
	GLint uniformUseInstancing;
};


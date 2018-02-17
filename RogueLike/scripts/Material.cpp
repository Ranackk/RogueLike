#include "stdafx.h"
#include "Material.h"
#include "Utility.h"
#include <gtx/dual_quaternion.hpp>
#include "TextureManager.h"
#include "Game.h"
#include "CameraComponent.h"
#include "CameraComponent.h"
#include "CameraComponent.h"
#include <mmintrin.h>
#include <detail/_vectorize.hpp>

std::map<std::string, GLuint> Material::compiledShaders;

Material::Material(std::string shaderName) {
	if (compiledShaders.find(shaderName) != compiledShaders.end()) {
		this->shaderProgramID = compiledShaders[shaderName];
	}
	else {
		std::cout << "There is no material with identifier " << shaderName.c_str() << " - Falling back to default!" << std::endl;
		return;
	}

	std::cout << "Created new material of shader name" << shaderName.c_str() << std::endl;
}

void Material::setupMatricesOnly() {
	m_Type = MATRICES_ONLY;

	/* Set up the uniform locations shared between all shaders */
	this->uniformModelMatrix = glGetUniformLocation(this->shaderProgramID, "_Model");
	this->uniformViewMatrix = glGetUniformLocation(this->shaderProgramID, "_View");
	this->uniformProjectionMatrix = glGetUniformLocation(this->shaderProgramID, "_Projection");

	this->uniformClippingPlane = glGetUniformLocation(this->shaderProgramID, "_ClippingPlane");
	this->uniformShadowClippingPlane = glGetUniformLocation(this->shaderProgramID, "_ShadowClippingPlane");

	this->uniformUseInstancing = glGetUniformLocation(this->shaderProgramID, "_UseInstancing");
}
void Material::setupBaseShader(const glm::vec4 _diffuseColor, const GLuint _textureID, const GLuint _skyboxID) {
	setupMatricesOnly();

	m_Type = BASE_SHADER;

	this->p_diffuseColor = _diffuseColor;
	this->p_textureID = _textureID;

	/* Setup the uniform locations for the base shader */
	this->uniformDiffuseColor = glGetUniformLocation(this->shaderProgramID, "_Diffuse");
	this->uniformTexture = glGetUniformLocation(this->shaderProgramID, "_Texture");

	/* Test Area */
	p_skyboxID = _skyboxID; 
	this->uniformSkybox = glGetUniformLocation(this->shaderProgramID, "_Skybox");
}
void Material::setupBaseUiShader(const GLuint _textureID) {
	setupMatricesOnly();

	m_Type = UI_BASE_SHADER;
	this->p_textureID = _textureID;
	this->uniformTexture = glGetUniformLocation(this->shaderProgramID, "_Texture");
}

void Material::setTexture(const GLuint texture) {
	p_textureID = texture;
}
void Material::setDiffuse(const glm::vec4 diffuse) {
	p_diffuseColor = diffuse;
}


void Material::compileShader(std::string path) {
	// If the table doesnt exist, create one
	if (&Material::compiledShaders == nullptr) {
		compiledShaders = std::map<std::string, GLuint>();
	}
	if (compiledShaders.find(path) != compiledShaders.end()) return;

	// Try to load a shader
	GLuint shaderId; 
	if (Utility::loadShader( path, shaderId)) {
		// If success: Put it into the table
		compiledShaders.insert(std::pair<std::string, GLuint>(path, shaderId));
		std::cout << "... Shader \"" << path.c_str() << "\" compiled at GLID = " << shaderId << std::endl;
	}
	else {
		// Else: Print error
		std::cerr << "Could not compile shader " << path.c_str() << std::endl;
	}
}

void Material::bindMaterial(glm::mat4x4 _perspectiveMatrix, glm::mat4x4 _viewMatrix, glm::mat4x4 _modelMatrix, const GLuint _vertexBufferID,
                            const GLuint _uvBufferID, const GLuint _normalBufferID, const bool _instanced) const {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "MatStart GL ERROR " << err << std::endl;
	}

	/* Buffers */

	// Bind Position Buffer
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(nullptr));

	// Bind UV Buffer
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, _uvBufferID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(nullptr));

	// Bind Normal Buffer
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, _normalBufferID);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(nullptr));

	// Bind Program
	glUseProgram(this->shaderProgramID);

	/* Uniforms */
	/* Material specific values */
	switch (m_Type) {
	case MATRICES_ONLY:
		break;
	case BASE_SHADER:
		// Setup Lights
		RenderEngine::prepareSceneLightsForShaderProgram(Game::getInstance()->getCurrentScene(), this->shaderProgramID);
		// Diffuse Color
		glUniform4f(this->uniformDiffuseColor, this->p_diffuseColor.x, this->p_diffuseColor.y, this->p_diffuseColor.z, this->p_diffuseColor.w);
		// Main Texture - Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->p_textureID);
		glUniform1i(this->uniformTexture, 0);
		// Skybox Texture - Texture Unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->p_skyboxID);
		glUniform1i(this->uniformSkybox, 1);
		break;
	case UI_BASE_SHADER:
		// Main Texture - Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->p_textureID);
		glUniform1i(this->uniformTexture, 0);
		break;
	}
	err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "Mat End GL ERROR " << err << std::endl;
	}
	/* Matrices */
	glUniformMatrix4fv(this->uniformModelMatrix, 1, GL_FALSE, &_modelMatrix[0][0]);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "Mat End GL ERROR " << err << std::endl;
	}
	glUniformMatrix4fv(this->uniformViewMatrix, 1, GL_FALSE, &_viewMatrix[0][0]);
	glUniformMatrix4fv(this->uniformProjectionMatrix, 1, GL_FALSE, &_perspectiveMatrix[0][0]);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "Mat End GL ERROR " << err << std::endl;
	}

	/* Clipping Plane */
	glUniform2f(this->uniformClippingPlane, Game::m_s_cNearClip, Game::m_s_cFarClip);
	/* Clipping Plane */
	glUniform2f(this->uniformShadowClippingPlane, Game::m_s_cShadowNearClip, Game::m_s_cShadowFarClip);

	/* Instancing */
	glUniform1i(this->uniformUseInstancing, _instanced);

	err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "Mat End GL ERROR " << err << std::endl;
	}
}

void Material::unbindMaterial() {
	// Unbind Program
	glUseProgram(0);
	// Unbind Buffers (inverse order than loading them)
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, 0);
	

}

GLuint Material::getShaderProgramId() const {
	return shaderProgramID;
}


Material::~Material() {}


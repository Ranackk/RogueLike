#pragma once
#include "stdafx.h"
#include <map>
#include "CameraComponent.h"

class Material
{
public:
	enum Type {
		MATRICES_ONLY, 
		BASE_SHADER, 
		UI_BASE_SHADER, 
		UI_HEART_CONTAINER
	};

	static void compileShader(std::string path);

	explicit Material(std::string shaderName, const Type _type);

	/* Setup for different types of materials */
	void setupMatricesOnly();
	void setupBaseShader(glm::vec4 _diffuseColor, GLuint _textureID, GLuint _skyboxID);
	void setupBaseUiShader(GLuint _textureID);
	void setupHealthBarShader(const GLuint _backgroundTextureID, const GLuint _fillTextureID,
	                          const GLuint _gradientTextureID, float* _fillAmount);

	void setTexture(const GLuint texture);
	void setDiffuse(const glm::vec4 diffuse);
	void setBlink(const float _blink);
	void setUvOffset(const glm::vec2 _offset);

	~Material();

	void bindMaterial(glm::mat4x4 _perspectiveMatrix, glm::mat4x4 _viewMatrix, glm::mat4x4 _modelMatrix,
		const GLuint _vertexBufferID, const GLuint _uvBufferID, const GLuint _normalBufferID, const bool _instanced) const;
	void unbindMaterial();
	
	GLuint getShaderProgramId() const;

private:
	static std::map<std::string, GLuint> compiledShaders;
	Type m_Type;

	GLuint shaderProgramID;

	/* Matrix, LightComponent Uniform GLuints */
	GLuint uniformModelMatrix;
	GLuint uniformViewMatrix;
	GLuint uniformProjectionMatrix;

	GLint uniformClippingPlane;
	GLint uniformShadowClippingPlane;
	GLint uniformUseInstancing;
	GLuint uniformGameTime;
	GLuint uniformCosGameTime;
	GLuint uniformCosGameTimeFast;

	/* === Custom Properties "BaseShader" === */
	/* Material Properties */
	glm::vec4 p_diffuseColor;
	GLuint p_textureID;

	/* Material Uniform GLuints */
	GLuint uniformDiffuseColor;
	GLuint uniformTexture;

	GLint uniformUvOffset;
	glm::vec2 p_uvOffset;

	GLuint p_skyboxID;
	GLuint uniformSkybox;

	/* === Properties "HealthBar" === */
	GLuint p_backgroundTextureID;
	GLuint p_fillTextureID;
	GLuint p_gradientTextureID;
	float* p_healthbarFillAmount;

	GLuint uniformBackgroundTexture;
	GLuint uniformFillTextue;
	GLuint uniformGradientTexture;
	GLuint uniformHealthbarFillAmount;
	GLuint uniformBlink;
	float p_Blink = 0.0f;
};


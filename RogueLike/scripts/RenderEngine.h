#pragma once
#include "stdafx.h"
#include "GameObject.h"

class CameraComponent;
class Scene;

class RenderEngine
{
public:
	RenderEngine();
	~RenderEngine();

	static GLFWwindow* createWindow();
	static GLuint createVertexArrayObjectVAO();

	static void copyCubeMapArray(GLuint staticShadowMapCubeTextureDepth, GLuint dynamicShadowMapCubeTextureDepth, int arrayLength);
	static void renderSceneFromCamera(Scene* _scene, CameraComponent* _camera);
	static void prerenderStaticShadowMaps(Scene* _scene, Material* _material);
	static void renderShadowMaps(Scene* _scene, Material* _material);

	static void prepareSceneLightsForShaderProgram(Scene* _scene, GLuint _shaderProgramID);
	static void createShadowFrameBufferObject(GLuint& fboHandle, GLuint& cubeMapHandle, int _lightCount);

	static glm::vec3 getCubeMapFaceDirection(const int face);
	static glm::vec3 getCubeMapUpVector(const int face);
private:
	GLuint m_VertexArrayObjectID;

	static const glm::mat4x4 m_s_cCubeMapPerspectiveMatrix;
};


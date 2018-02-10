#include "stdafx.h"
#include "RenderEngine.h"
#include "Game.h"
#include <gtc/matrix_transform.inl>
#include <detail/_vectorize.hpp>
#include <string>
#include <detail/_vectorize.hpp>
#include <detail/_vectorize.hpp>
#include <detail/_vectorize.hpp>

/* Those callbacks may not live in a class */
void onWindowClose(GLFWwindow* window);
void onWindowResize(GLFWwindow* window, int width, int height);
void onGlfwError(int error, const char* description);


const glm::mat4x4 RenderEngine::m_s_cCubeMapPerspectiveMatrix = glm::perspective(glm::radians(90.0f), 1.0f, Game::m_s_cNearClip, Game::m_s_cFarClip);

RenderEngine::RenderEngine()
{
	m_VertexArrayObjectID = -1;
}


RenderEngine::~RenderEngine()
{
}

GLFWwindow* RenderEngine::createWindow() {
	/* Initialize GLFW */
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
	}

	glfwSetErrorCallback(static_cast<GLFWerrorfun>(onGlfwError));

	/* Setup the glfw window Hints [They deprecate glBegin(), so while no VBOs & VAOs are implemented, I dont use this]*/
	glfwWindowHint(GLFW_SAMPLES, 4);								// 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					// OpenGL 4.1 (was 3.3 (core) at the start)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			// To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);	// We don't want the old OpenGL 

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

	/* Create glfw Window */
	GLFWwindow* window = glfwCreateWindow(Game::m_s_cWindowWidth, Game::m_s_cWindowHeight, Game::m_s_cWindowTitle.c_str(), nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "Failed to open GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	/* Setup glfw callbacks */
	glfwSetWindowCloseCallback(window, static_cast<GLFWwindowclosefun>(onWindowClose));
	glfwSetWindowSizeCallback(window, static_cast<GLFWwindowsizefun>(onWindowResize));

	/* Make contect current */
	glfwMakeContextCurrent(window);

	/* Hide mouse & center to mid */
	glfwSetCursorPos(window, Game::m_s_cWindowWidth / 2, Game::m_s_cWindowHeight / 2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	/* Initialize glew */
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initiialize GLEW" << std::endl;
	}
	glViewport(0, 0, Game::m_s_cWindowWidth, Game::m_s_cWindowHeight);

	/* Create VAO (doesnt save VAO id for now) */
	createVertexArrayObjectVAO();

	/* GL Flags */
	/* Enable Depth Test with func LEqual*/
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	/* Enable Back-Facde Culling*/
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	const int glErrorCode = glGetError();
	if (glErrorCode != 0) {
		std::cerr << "... Failed to setup all GL-Flags: Error Code is " << glErrorCode << std::endl;
	}
	else {
		/* Get Open GL Version */
		std::cout << "... Window successfully set up [" << Game::m_s_cWindowWidth << "x " << Game::m_s_cWindowHeight << "], OPEN GL Version is " << glGetString(GL_VERSION) << ", Clipping Planes: " << Game::m_s_cNearClip << ", " << Game::m_s_cFarClip << std::endl;
		
		/* Get Maximum Varyings */
		int n;
		glGetIntegerv(GL_MAX_VARYING_FLOATS, &n);

		std::cout << "Maxiumum Varying Floats: " << n << std::endl;

	}


	return window;
}

GLuint RenderEngine::createVertexArrayObjectVAO() {
	/* Create VAO */
	GLuint m_VAO;
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	std::cout << "... Generated VAO at ID " << m_VAO << std::endl;

	return m_VAO;
}

void RenderEngine::copyCubeMapArray(const GLuint staticShadowMapCubeTextureDepth,
                                    const GLuint dynamicShadowMapCubeTextureDepth, const int arrayLength) {
	glCopyImageSubData(staticShadowMapCubeTextureDepth, GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, 0,
		dynamicShadowMapCubeTextureDepth, GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, 0,
		256, 256, arrayLength * 6);

	const GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "Error: " << err << std::endl;
	}
}

void RenderEngine::renderSceneFromCamera(Scene* _scene, Camera* _camera) {
	glEnable(GL_DEPTH_TEST);
	//glCullFace(GL_FRONT);

	/* Copy the static shadowmaps into the dynamic shadow maps slot so that they can add dynamic objects */
	//copyCubeMapArray(_scene->m_StaticShadowMapCubeTextureDepth, _scene->m_DynamicShadowMapCubeTextureDepth, _scene->getLightCount());

	/* Draw the shadows of all Lights */
	_scene->drawDynamicShadowMaps();

	/* Draw the whole scene */
	//glCullFace(GL_BACK);
	_scene->drawFull(_camera->m_ProjectionMatrix, _camera->getViewMatrix());
	glDisable(GL_DEPTH_TEST);
}


void RenderEngine::prerenderStaticShadowMaps(Scene* _scene, Material* _material) {
	/* Create perspective matrix with a FOV of 90, AspectRation of 1, zNear & zFar */
	const glm::mat4 perspectiveMatrix = glm::perspective(glm::radians(90.0f), 1.0f, Game::m_s_cNearClip, Game::m_s_cFarClip);
	std::vector<Light*> lights = _scene->getLights();

	/* Start Rendering to the FBO */
	glBindFramebuffer(GL_FRAMEBUFFER, _scene->m_StaticShadowMapFBO);

	/* Set the viewport size */
	glViewport(0, 0, Game::m_s_cShadowMapResolution, Game::m_s_cShadowMapResolution);

	/* Clear the existing depth buffer */
	glClear(GL_DEPTH_BUFFER_BIT);

	/* Loop through each scene light (that is the scenes light count minus 1 as the last one is the players light */
	for (int lightID = 0; lightID < _scene->getLightCount(); lightID++) {
		/* If the light is not static, it doesnt render into the static cube map*/
		if (!lights[lightID]->isStatic()) continue;

		/* Loop through each face */
		const glm::mat4x4* viewMatrices = lights[lightID]->getCubeMapViewMatrices();
		for (int faceID = 0; faceID < 6; faceID ++) {
			const glm::mat4 viewMatrix = viewMatrices[faceID];

			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _scene->m_StaticShadowMapCubeTextureDepth, 0, lightID * 6 + faceID);

			GLenum err = glGetError();
			if (err != GL_NO_ERROR) {
				std::cout << "Error: " << err << std::endl;
			}
			_scene->drawStaticShadowCasters(perspectiveMatrix, viewMatrix, _material);
		}
	}

	/* Set the render target to the screen */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/* Set the viewport size */
	glViewport(0, 0, Game::m_s_cWindowWidth, Game::m_s_cWindowHeight);
}

void RenderEngine::renderShadowMaps(Scene* _scene, Material* _material) {
	/* Create perspective matrix with a FOV of 90, AspectRation of 1, zNear & zFar */
	const glm::mat4 perspectiveMatrix = glm::perspective(glm::radians(90.0f), 1.0f, Game::m_s_cNearClip, Game::m_s_cFarClip);
	std::vector<Light*> lights = _scene->getLights();

	/* Start Rendering to the static shadow map FBO */
	glBindFramebuffer(GL_FRAMEBUFFER, _scene->m_StaticShadowMapFBO);

	/* Set the viewport size */
	glViewport(0, 0, Game::m_s_cShadowMapResolution, Game::m_s_cShadowMapResolution);

	/* Loop through all dynamic lights to let them render the static objects in the scene */
	for (int lightID = 0; lightID < _scene->getLightCount(); lightID++) {
		if (lights[lightID]->isStatic()) continue;
		/* Loop through each face */
		const glm::mat4x4* viewMatrices = lights[lightID]->getCubeMapViewMatrices();
		for (int faceID = 0; faceID < 6; faceID++) {
			const glm::mat4 viewMatrix = viewMatrices[faceID];

			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _scene->m_StaticShadowMapCubeTextureDepth, 0, lightID * 6 + faceID);
			/* Clear the texture */
			glClear(GL_DEPTH_BUFFER_BIT);

			GLenum err = glGetError();
			if (err != GL_NO_ERROR) {
				std::cout << "Error: " << err << std::endl;
			}

			_scene->drawStaticShadowCasters(perspectiveMatrix, viewMatrix, _material);
		}
	}

	/* Set the render target to the screen */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/* Set the viewport size */
	glViewport(0, 0, Game::m_s_cWindowWidth, Game::m_s_cWindowHeight);




	/* Start Rendering to the dynamic shadow map FBO */
	glBindFramebuffer(GL_FRAMEBUFFER, _scene->m_DynamicShadowMapFBO);

	/* Set the viewport size */
	glViewport(0, 0, Game::m_s_cShadowMapResolution, Game::m_s_cShadowMapResolution);

	/* Loop through all lights to let them render the dynamic objects in the scene */
	for (int lightID = 0; lightID < _scene->getLightCount(); lightID++) {
		/* Loop through each face */
		const glm::mat4x4* viewMatrices = lights[lightID]->getCubeMapViewMatrices();
		for (int faceID = 0; faceID < 6; faceID++) {

			const glm::mat4 viewMatrix = viewMatrices[faceID];

			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _scene->m_DynamicShadowMapCubeTextureDepth, 0, lightID * 6 + faceID);
			/* Clear the texture */
			glClear(GL_DEPTH_BUFFER_BIT);

			GLenum err = glGetError();
			if (err != GL_NO_ERROR) {
				std::cout << "Error: " << err << std::endl;
			}

			//_scene->drawStaticShadowCasters(perspectiveMatrix, viewMatrix, _material);
			_scene->drawDynamicShadowCasters(perspectiveMatrix, viewMatrix, _material);
		}
	}

	/* Set the render target to the screen */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/* Set the viewport size */
	glViewport(0, 0, Game::m_s_cWindowWidth, Game::m_s_cWindowHeight);
}


void RenderEngine::createShadowFrameBufferObject(GLuint &fboHandle, GLuint &cubeMapHandle, const int _lightCount) {
	/* FBO */
	glGenFramebuffers(1, &fboHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

	/* Depth Cube Map */

	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &cubeMapHandle);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, cubeMapHandle);

	/* Set paramteres */
	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT16, 256, 256, _lightCount * 6, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	//for (int faceID = 0; faceID < 6; faceID++) {
	//	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceID, 0, GL_DEPTH_COMPONENT16, 256, 256, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	//}

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMapHandle, 0);
	glDrawBuffer(GL_NONE); // No color buffer is drawn to.

						   // Always check that our framebuffer is ok
	const GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Shadow FBO creation created error: " << fboStatus << std::endl;

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "Create FBO GL ERROR " << err << std::endl;
	}
	else {
		std::cout << "Created Shadow FBO with ID " << fboHandle << " & depthCubeMapID " << cubeMapHandle << std::endl;
	}
}

glm::vec3 RenderEngine::getCubeMapFaceDirection(const int face) {
	switch (face) {
		// posX
	case 1: return glm::vec3(-1, 0, 0);
		// negX
	case 0: return glm::vec3(1, 0, 0);
		// posY
	case 2: return glm::vec3(0, -1, 0);
		// negY
	case 3: return glm::vec3(0, 1, 0);
		// posZ
	case 4: return glm::vec3(0, 0, -1);
		// negZ
	case 5: return glm::vec3(0, 0, 1);

	default: return glm::vec3(0, 0, 0);
	}
}

glm::vec3 RenderEngine::getCubeMapUpVector(const int face) {
	switch (face) {
		// posY
	case 2: return glm::vec3(0, 0, -1);
		// negY
	case 3: return glm::vec3(0, 0, 1);
		// everything else
	default: return glm::vec3(0, 1, 0);
	}
}

void RenderEngine::prepareSceneLightsForShaderProgram(Scene* _scene, const GLuint _shaderProgramID) {
	_scene->prepareLightsForShaderProgram(_shaderProgramID);
}

/* Error Callback implementation */
void onWindowClose(GLFWwindow* window) {
	Game::getInstance()->stopGame();
}

void onWindowResize(GLFWwindow* window, int width, int height) {
	/* Set the viewport to span form 0|0 to width|height */
	glViewport(0, 0, width, height);
}

void onGlfwError(int error, const char* description) {
	std::cerr << "GLFW-ERROR: " << description << "ERROR-ID:" << error << std::endl;
}




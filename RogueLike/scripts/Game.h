#pragma once

#include "stdafx.h"
#include "TextureManager.h"
#include "Scene.h"
#include "Camera.h"
#include "ModelManager.h"
#include "RenderEngine.h"
#include "MaterialManager.h"
#include "PlayerComponent.h"

class Game
{
public:
	/* Static constants */
	static const int m_s_cWindowWidth = 960;
	static const int m_s_cWindowHeight = 540;

	static const float m_s_cNearClip;
	static const float m_s_cFarClip;
	static const std::string m_s_cWindowTitle;

	static const int m_s_cRoomsPerRow = 1;
	static const int m_s_cRoomsPerColumn = 1;

	static const int m_s_cRoomWidthInFields = 13;
	static const int m_s_cRoomHeightInFields = 9;

	static const int m_s_cFieldPixelSize = 32;

	static const int m_s_cMipMapMaxLevel = 4;

	static const int m_s_cShadowMapResolution = 256;

	void createStaticShadowMaps() const;
	Game();
	~Game();

	static Game* getInstance();    
	void operator=(Game const&)		= delete;	

	//void prepareLightsForShaderProgram(const GLuint _shaderProgramID);

	int startGame();
	void stopGame();
	void setupSystems() const;

	TextureManager* getTextureManager() const;
	ModelManager* getModelManager() const;
	MaterialManager* getMaterialManager() const;
	Scene* getCurrentScene() const;

private:
	static Game* m_s_Instance;
	bool m_Running = true;

	TextureManager* m_TextureManager;
	ModelManager* m_ModelManager;
	MaterialManager* m_MaterialManager;

	GLFWwindow* m_Window;

	Camera* m_Camera;
	Scene* m_Scene;

	//GLuint m_LightShaderProgrammID;
	//int m_LightCount;
	//Light* m_Lights;

	//void initializeWindow();
	//void createVAO();
	void initializeManagers();
	void initializeGameComponents();

	void draw() const;
	void update(double ellapsedTime);

};


#pragma once

#include "stdafx.h"
#include "TextureManager.h"
#include "Scene.h"
#include "CameraComponent.h"
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

	static const int m_s_cRoomsPerRow = 7;
	static const int m_s_cRoomsPerColumn = 7;

	static const int m_s_cRoomWidthInFields = 13;
	static const int m_s_cRoomHeightInFields = 9;
	static const int m_s_cSeed = 2;

	static const int m_s_cFieldPixelSize = 32;

	static const int m_s_cMipMapMaxLevel = 4;

	static const float m_s_cShadowNearClip;
	static const float m_s_cShadowFarClip;
	static const float m_s_cLightActiveDistance;
	static const int m_s_cShadowMapResolution = 256 * 2;		//Performance tests (2 lights): 256 -> 5.7ms, 512 -> 6.5ms

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
	float getGameTime() const;

private:
	static Game* m_s_Instance;
	bool m_Running = true;

	TextureManager* m_TextureManager;
	ModelManager* m_ModelManager;
	MaterialManager* m_MaterialManager;

	GLFWwindow* m_Window;

	CameraComponent* m_Camera;
	Scene* m_Scene;

	float m_GameTime;

	void initializeManagers();
	void initializeGameComponents();

	void draw() const;
	void update(double ellapsedTime);

};


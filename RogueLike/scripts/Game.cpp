#include "stdafx.h"
#include "Game.h"
#include <cassert>
#include "Material.h"


class PlayerComponent;
using namespace std;

/*
 * The games main class
 */

Game* Game::m_s_Instance = nullptr;
const float Game::m_s_cNearClip = 0.1f;
const float Game::m_s_cFarClip = 70.0f;
const float Game::m_s_cShadowNearClip = 0.01f;
const float Game::m_s_cShadowFarClip = 30.0f;
const float Game::m_s_cLightActiveDistance = 23.0f;


const string Game::m_s_cWindowTitle = "Ranogue Engine!";

Game::Game()
{
	m_s_Instance = this;
	cout << "Initializing Game... " << endl;
	this->initializeManagers();
	cout << "> Initialized Managers" << endl;
	this->initializeGameComponents();
	cout << "> Initialized Game Components" << endl;
	this->setupSystems();
	cout << "> Systems Setup!" << endl;
	this->createStaticShadowMaps();
	cout << "> Static Shadow Maps drawn!" << endl;
	cout << "... Initialization finished! " << endl;
	cout << "=== Setup Done ===" << endl;
}
Game::~Game()
{
	delete m_TextureManager;
	delete m_ModelManager;
	delete m_Camera;
	delete m_Scene;
}

Game* Game::getInstance()
{
	return m_s_Instance;
}

int Game::startGame() 
{
	cout << "Starting Game Loop ..." << endl;
	double lastTime = 0;
	double ellapsedTime = 0;
	double currentTime = 0;


	double lastSecond = 0;
	int nbFrames = 0;

	int seconds = 0;
	int totalnbFrames = 0;

	while (m_Running) {
		if (glfwWindowShouldClose(m_Window)) {
			m_Running = false;
		}

		/* Get the ellapsed time */
		currentTime = glfwGetTime();
		nbFrames++;
		totalnbFrames++;
		if (currentTime > lastSecond + 1.0) {
			cout << (1000.0 / (double)nbFrames) << "ms (FPS: " << nbFrames << ") ||| av: " << ((1000.0 * seconds) / (double)totalnbFrames) << " ms" << endl;
			nbFrames = 0;
			lastSecond += 1.0;

			seconds++;

		}
		ellapsedTime = currentTime - lastTime;
		lastTime = currentTime;

		/* Update */
		update(ellapsedTime);

		/* Clear current buffer */
		glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Render */
		draw();

		/* Swap back & front buffer */
		glfwSwapBuffers(m_Window);

		/* Poll events */
		glfwPollEvents();

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::cerr << "Open GL Error: " << err << std::endl;
			break;
		}
	}

	glfwDestroyWindow(m_Window);
	glfwTerminate();

	return 0;
}

void Game::stopGame() {
	m_Running = false;
}

void Game::initializeManagers() {
	// Let the RenderEngine create a window
	m_Window = RenderEngine::createWindow();

	/* Load up all needed textures */
	m_TextureManager = new TextureManager();
	// MAP
	m_TextureManager->loadTextureAs("graphics/map/stone_02.png", FieldType::WALL_X.getTextureIdentifier());
	m_TextureManager->loadTextureAs("graphics/map/earth_01.png", FieldType::FLOOR.getTextureIdentifier());
	m_TextureManager->loadTextureAs("graphics/map/water_03.png", FieldType::WATER.getTextureIdentifier());
	// ENTITIES
	m_TextureManager->loadTextureAs("graphics/player.png", "tex_Player");
	m_TextureManager->loadTextureAs("graphics/bullet.png", "tex_Projectile");

	m_TextureManager->loadTextureAs("graphics/enemy_02.png", "tex_Enemy_Rogue");
	m_TextureManager->loadTextureAs("graphics/enemy_01.png", "tex_Enemy_Archer");

	m_TextureManager->loadTextureAs("graphics/barrier.png", "tex_Barrier");
	// UI
	m_TextureManager->loadTextureAs("graphics/ui/heartcontainerBackground.png", "tex_UI_HeartContainer_BG");
	m_TextureManager->loadTextureAs("graphics/ui/heartcontainerFill.png", "tex_UI_HeartContainer_Fill");
	m_TextureManager->loadTextureAs("graphics/ui/heartcontainerGradient.png", "tex_UI_HeartContainer_Gradient");

	std::cout << "=====" << std::endl;

	/* Load up all needed materials */
	m_MaterialManager = new MaterialManager();
	// TEC
	m_MaterialManager->createMaterialByShader("depthShader", "mat_TEC_RenderDistanceToNearestSurfaceIntoDepthBuffer", Material::MATRICES_ONLY);
	// MAP
	m_MaterialManager->createMaterialByShader("baseShader", "mat_Wall", Material::BASE_SHADER);
	m_MaterialManager->createMaterialByShader("baseShader", "mat_Floor", Material::BASE_SHADER);
	m_MaterialManager->createMaterialByShader("baseShader", "mat_Water", Material::BASE_SHADER);
	// ENTITIES
	m_MaterialManager->createMaterialByShader("baseShader", "mat_Player", Material::BASE_SHADER);
	m_MaterialManager->createMaterialByShader("baseShader", "mat_Enemy_Rogue", Material::BASE_SHADER);
	m_MaterialManager->createMaterialByShader("baseShader", "mat_Enemy_Archer", Material::BASE_SHADER);
	m_MaterialManager->createMaterialByShader("baseShader", "mat_Projectile", Material::BASE_SHADER);

	m_MaterialManager->createMaterialByShader("baseShader", "mat_Barrier", Material::BASE_SHADER);
	// UI
	m_MaterialManager->createMaterialByShader("baseUIShader", "mat_UITest", Material::UI_BASE_SHADER);

	m_MaterialManager->createMaterialByShader("uiHeartContainerShader", "mat_UIHeartContainer0", Material::UI_HEART_CONTAINER);
	m_MaterialManager->createMaterialByShader("uiHeartContainerShader", "mat_UIHeartContainer1", Material::UI_HEART_CONTAINER);
	m_MaterialManager->createMaterialByShader("uiHeartContainerShader", "mat_UIHeartContainer2", Material::UI_HEART_CONTAINER);
	m_MaterialManager->createMaterialByShader("uiHeartContainerShader", "mat_UIHeartContainer3", Material::UI_HEART_CONTAINER);
	m_MaterialManager->createMaterialByShader("uiHeartContainerShader", "mat_UIHeartContainer4", Material::UI_HEART_CONTAINER);
	m_MaterialManager->createMaterialByShader("uiHeartContainerShader", "mat_UIHeartContainer5", Material::UI_HEART_CONTAINER);
	m_MaterialManager->createMaterialByShader("uiHeartContainerShader", "mat_UIHeartContainer6", Material::UI_HEART_CONTAINER);
	m_MaterialManager->createMaterialByShader("uiHeartContainerShader", "mat_UIHeartContainer7", Material::UI_HEART_CONTAINER);
	m_MaterialManager->createMaterialByShader("uiHeartContainerShader", "mat_UIHeartContainer8", Material::UI_HEART_CONTAINER);
	m_MaterialManager->createMaterialByShader("uiHeartContainerShader", "mat_UIHeartContainer9", Material::UI_HEART_CONTAINER);

	std::cout << "=====" << std::endl;

	/* Load up all the models needed */
	m_ModelManager = new ModelManager();
	// MAP
	m_ModelManager->loadModelAs("models/map/floor.obj", "mesh_Floor");
	m_ModelManager->loadModelAs("models/map/water.obj", "mesh_Water");

	m_ModelManager->loadModelAs("models/map/Wall_basic_x.obj", "mesh_Wall_basic_x");
	m_ModelManager->loadModelAs("models/map/Wall_basic_z.obj", "mesh_Wall_basic_z");

	m_ModelManager->loadModelAs("models/map/Wall_arch_left_x.obj", "mesh_Wall_arch_left_x");
	m_ModelManager->loadModelAs("models/map/Wall_arch_left_z.obj", "mesh_Wall_arch_left_z");
	m_ModelManager->loadModelAs("models/map/Wall_arch_right_x.obj", "mesh_Wall_arch_right_x");
	m_ModelManager->loadModelAs("models/map/Wall_arch_right_z.obj", "mesh_Wall_arch_right_z");

	m_ModelManager->loadModelAs("models/map/Wall_corner_left_back.obj", "mesh_Wall_corner_left_back");
	m_ModelManager->loadModelAs("models/map/Wall_corner_right_back.obj", "mesh_Wall_corner_right_back");
	m_ModelManager->loadModelAs("models/map/Wall_corner_left_front.obj", "mesh_Wall_corner_left_front");
	m_ModelManager->loadModelAs("models/map/Wall_corner_right_front.obj", "mesh_Wall_corner_right_front");

	m_ModelManager->loadModelAs("models/map/Wall_column_pos_x.obj",  "mesh_Wall_column_pos_x");
	m_ModelManager->loadModelAs("models/map/Wall_column_neg_x.obj", "mesh_Wall_column_neg_x");
	m_ModelManager->loadModelAs("models/map/Wall_column_pos_z.obj", "mesh_Wall_column_pos_z");
	m_ModelManager->loadModelAs("models/map/Wall_column_neg_z.obj", "mesh_Wall_column_neg_z");

	m_ModelManager->loadModelAs("models/map/Wall_window_x.obj", "mesh_Wall_window_x");
	m_ModelManager->loadModelAs("models/map/Wall_window_z.obj", "mesh_Wall_window_z");

	m_ModelManager->loadModelAs("models/map/Wall_broken_x.obj", "mesh_Wall_broken_x");
	m_ModelManager->loadModelAs("models/map/Wall_broken_z.obj", "mesh_Wall_broken_z");

	m_ModelManager->loadModelAs("models/map/rocks.obj", "mesh_Rocks");

	m_ModelManager->loadModelAs("models/map/Shrine.obj", "mesh_Shrine");

	m_ModelManager->loadModelAs("models/map/water.obj", "mesh_Water");
	// ENTITES
	m_ModelManager->loadModelAs("models/enemy_02.obj", "mesh_Player");
	m_ModelManager->loadModelAs("models/enemy_01.obj", "mesh_Enemy_Rogue");
	m_ModelManager->loadModelAs("models/enemy_02.obj", "mesh_Enemy_Archer");

	m_ModelManager->loadModelAs("models/player.obj", "mesh_Projectile");

	m_ModelManager->loadModelAs("models/trapdoor_x.obj", "mesh_Barrier_x");
	m_ModelManager->loadModelAs("models/trapdoor_z.obj", "mesh_Barrier_z");

}

void Game::initializeGameComponents() {
	this->m_Scene = new Scene(glm::vec2(m_s_cRoomsPerRow, m_s_cRoomsPerColumn));

	GameObject* gO = new GameObject("Camera");
	this->m_Camera = gO->addComponent(new CameraComponent());
	this->m_Camera->initialize(CameraComponent::Mode::FOLLOW_ROOM, this->m_Scene->m_Player);


}

void Game::setupSystems() const {
	this->m_Scene->setupSystems();
}


void Game::createStaticShadowMaps() const {
	/* Create static shadow maps */
	m_Scene->drawStaticShadowMaps();
}


void Game::draw() const {
	RenderEngine::renderSceneFromCamera(m_Scene, m_Camera);
}

void Game::update(double ellapsedTime) {
	if (m_Camera->getMode() != CameraComponent::LOCKED && m_Camera->getMode() != CameraComponent::FREE_PAUSED) {
		m_GameTime += (float) ellapsedTime;

		m_Scene->update(m_Window, (float) ellapsedTime);
	}
	m_Camera->getGameObject()->update(m_Window, (float) ellapsedTime);
}

TextureManager* Game::getTextureManager() const {
	return m_TextureManager;
}

ModelManager* Game::getModelManager() const {
	return m_ModelManager;
}

MaterialManager* Game::getMaterialManager() const {
	return m_MaterialManager;
}

Scene* Game::getCurrentScene() const {
	return m_Scene;
}

float Game::getGameTime() const {
	return m_GameTime;
}

#include "stdafx.h"
#include "Game.h"
#include <cassert>
#include "Material.h"


class Player;
using namespace std;

/*
 * The games main class
 */

Game* Game::m_s_Instance = nullptr;

const string Game::m_s_cWindowTitle = "Ranogue!";

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
		if (glfwWindowShouldClose(m_Window) || glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
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

	// Load up all needed textures
	m_TextureManager = new TextureManager();
	m_TextureManager->loadTextureAs("graphics/stone.png", FieldType::WALL.toString());
	m_TextureManager->loadTextureAs("graphics/demoTexture.png", FieldType::FLOOR.toString());

	// Load up all needed materials
	m_MaterialManager = new MaterialManager();
	m_MaterialManager->createMaterialByShader("baseShader", "wall", Material::BASE_SHADER);
	m_MaterialManager->createMaterialByShader("baseShader", "floor", Material::BASE_SHADER);
	m_MaterialManager->createMaterialByShader("depthShader", "depthMaterial", Material::MATRICES_ONLY);
	//m_MaterialManager->createMaterialByShader("depthShader", "depthMaterial", Material::MATRICES_ONLY);
	//m_MaterialManager->createMaterialByShader("depthColorShader", "depthColorMaterial", Material::MATRICES_ONLY);
	//Material::compileShader("baseShader");
	//Material::compileShader("depthShader");

	// Load up all the models needed
	m_ModelManager = new ModelManager();
	m_ModelManager->loadModelAs("models/player.obj", "player");
	m_ModelManager->loadModelAs("models/baseFloor.obj", "baseFloor");
	//m_ModelManager->loadModelAs("models/baseFloor.obj", "player");
	m_ModelManager->loadModelAs("models/baseWall.obj", "baseWall");

}

void Game::initializeGameComponents() {
	this->m_Camera = new Camera();
	this->m_Camera->initialize();

	this->m_Scene = new Scene(glm::vec2(4, 4));

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
	m_Scene->update(m_Window, ellapsedTime);
	m_Camera->update(m_Window, ellapsedTime);
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

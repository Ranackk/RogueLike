#include "stdafx.h"
#include "Scene.h"
#include "Game.h"
#include <math.h>
#include "WorldGenerator.h"
#include "Collider.h"
#include "PhysicsEngine.h"
#include "BoxColliderComponent.h"
#include "CircleCollider.h"
#include "CircleColliderComponent.h"
#include "GameObjectPool.h"

// Does not create the map yet!
Scene::Scene() {
	m_Fields = nullptr;

	//m_LightCount = 0;
	m_Lights = std::vector<LightComponent*>();
	m_LightShaderProgrammID = 0;
}

Scene::Scene(const glm::vec2 size) {
	this->m_Lights = std::vector<LightComponent*>();
	this->m_Enemies = std::vector<EnemyComponent*>();

	/* Setup PlayerComponent */
	GameObject* gO = new GameObject("Player");
	this->m_Player = gO->addComponent(new PlayerComponent());
	this->m_Player->initialize();
	this->m_Lights.push_back(m_Player->m_Light);

	/* Setup fields */
	m_RoomsPerMap = size;
	m_FieldSize = glm::vec2(m_RoomsPerMap.x * Game::m_s_cRoomWidthInFields, m_RoomsPerMap.y * Game::m_s_cRoomHeightInFields);
	generateMap(m_RoomsPerMap); 

	/* Shadow Mapping */
	m_DepthMaterial = Game::getInstance()->getMaterialManager()->getMaterialByName("mat_TEC_RenderDistanceToNearestSurfaceIntoDepthBuffer");
	m_DepthMaterial->setupMatricesOnly();

	RenderEngine::createShadowFrameBufferObject(m_StaticShadowMapFBO, m_StaticShadowMapCubeTextureDepth, m_Lights.size());
	RenderEngine::createShadowFrameBufferObject(m_DynamicShadowMapFBO, m_DynamicShadowMapCubeTextureDepth, m_Lights.size());


}

/* Draws the static shadow maps for all static lights */
void Scene::drawStaticShadowMaps() {
	RenderEngine::prerenderStaticShadowMaps(this, m_DepthMaterial.get());
}

/* Draws the dynamic shadow maps for all lights + the static shadow maps for dynamic lights */
void Scene::drawDynamicShadowMaps() {
	RenderEngine::renderShadowMaps(this, m_DepthMaterial.get());
}

Scene::~Scene()
{
	for (auto it = m_Lights.begin(); it < m_Lights.end(); ++it) {
		delete *it;
	}
	delete[] m_Fields;
}

void Scene::setupSystems() {
	// TODO: ADD VECTOR OF PHYSICS COMPONENTS -> Fields, Entities (Projectile, EnemyComponent)

	/* Static Rendering */
	m_StaticRenderComponents = std::vector<RenderComponent*>();
	for (unsigned int i = 0; i < m_FieldBatches.size(); i++) {
		m_StaticRenderComponents.push_back(m_FieldBatches[i].getComponent<RenderComponent>());
	}

	/* Dynamic Rendering */
	// Enemies
	m_DynamicRenderComponents = std::vector<RenderComponent*>();
	// Basic
	//for (unsigned int i = 0; i < m_Enemies.size(); i++) {
	//	m_DynamicRenderComponents.push_back(m_Enemies[i]->getComponent<RenderComponent>());
	//}

	// Batched
	m_EnemyPools = std::vector<GameObjectPool>();
	// Setup pool for all enemies (all same for now)
	{
		GameObjectPool enemyPool = GameObjectPool();
		RenderComponent* rc = m_Enemies[0]->getGameObject()->getComponent<RenderComponent>();
		enemyPool.initialize(128, rc->getModelData(), rc->getMaterial());
		std::vector<GameObject*> enemyGOs;
		for (int i = 0; i < m_Enemies.size(); i++) {
			enemyGOs.push_back(m_Enemies[i]->getGameObject());
		}
		enemyPool.initWithGameObjectVector(enemyGOs);
		enemyPool.updateRenderBatch();

		m_EnemyPools.push_back(enemyPool);
	}

	// Add all pools to rendering
	for (int i = 0; i < m_EnemyPools.size(); i++) {
		m_DynamicRenderComponents.push_back(m_EnemyPools[i].getRenderBatch().getComponent<RenderComponent>());
	}
	

	// Rendering
	m_DynamicRenderComponents.push_back(m_Player->getGameObject()->getComponent<RenderComponent>());


	/* HUD Rendering */
	m_HudRenderComponents = std::vector<HUDRenderComponent*>();
	
	m_TestUI = new GameObject();

	std::shared_ptr<Material> uiMat = Game::getInstance()->getMaterialManager()->getMaterialByName("mat_UITest");
	uiMat->setupBaseUiShader(Game::getInstance()->getTextureManager()->getTextureByIdentifier("tex_Player"));

	HUDRenderComponent* uiComp = new HUDRenderComponent();
	m_TestUI->addComponent(uiComp);
	uiComp->initialize(uiMat, glm::vec2(0.9f, 0.8f), glm::vec2(.1f, .2f));

	m_HudRenderComponents.push_back(uiComp);
}

void Scene::update(GLFWwindow* window, const float deltaTime) {
	m_Player->getGameObject()->update(window, deltaTime);
	for (auto it = m_Lights.begin(); it < m_Lights.end(); ++it) {
		(*it)->getGameObject()->update(window, deltaTime);
	}

	for (int i = 0; i < m_EnemyPools.size(); i++) {
		m_EnemyPools[i].getRenderBatch().update(window, deltaTime);
	}
}

/* Draws the whole scene to the screen */
void Scene::drawFull(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial) {

	/* Draw static geometry */
	drawStaticShadowCasters(_perspectiveMatrix, _viewMatrix, customMaterial);

	/* Draw dynamic geometry */
	drawDynamicShadowCasters(_perspectiveMatrix, _viewMatrix, customMaterial);

	/* Draw HUD */
	drawHudElements(_perspectiveMatrix, _viewMatrix, customMaterial);

}

/* Draws all static objects in the scene that cast shadows */
void Scene::drawStaticShadowCasters(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial) {
	for (unsigned int i = 0; i < m_StaticRenderComponents.size(); i++) {
		m_StaticRenderComponents[i]->draw(_perspectiveMatrix, _viewMatrix, customMaterial);
	}
}

/* Draws all dynamic objects in the scene that cast shadows */
void Scene::drawDynamicShadowCasters(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial) {
	for (unsigned int i = 0; i < m_DynamicRenderComponents.size(); i++) {
		m_DynamicRenderComponents[i]->draw(_perspectiveMatrix, _viewMatrix, customMaterial);
	}
}

/* Draws all hud elements in the scene */
void Scene::drawHudElements(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial) {
	for (unsigned int i = 0; i < m_HudRenderComponents.size(); i++) {
		m_HudRenderComponents[i]->draw(_perspectiveMatrix, _viewMatrix, customMaterial);
	}
}



void Scene::prepareLightsForShaderProgram(const GLuint _shaderProgramID) const {
	// TODO: do all this generation stuff at scene initialize, not every frame
	std::vector<float> ranges;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec4> colors;
	int amount = 0;

	/* Lights*/
	for (auto it = m_Lights.begin(); it < m_Lights.end(); ++it) {
		float range;
		glm::vec3 position;
		glm::vec4 color;
		if ((*it)->prepareForRender(range, position, color)) {
			ranges.push_back(range);
			positions.push_back(position);
			colors.push_back(color);
		}
	}

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "LightPrep-start GL ERROR " << err << std::endl;
	}
	//std::cout << "... " << amount << " Lights eligable " << std::endl;
	glUniform1i(glGetUniformLocation(_shaderProgramID, "_LightCount"), m_Lights.size());
	glUniform1fv(glGetUniformLocation(_shaderProgramID, "_LightRanges"), m_Lights.size(), &ranges[0]);
	glUniform3fv(glGetUniformLocation(_shaderProgramID, "_LightPositions"), m_Lights.size(), &positions[0].x);
	glUniform4fv(glGetUniformLocation(_shaderProgramID, "_LightColors"), m_Lights.size(), &colors[0].x);

	// Shadow Map Texture - Texture Unit 2 & 3
	glUniform1i(glGetUniformLocation(_shaderProgramID, "_LightStaticShadowMaps"), 2);
	glUniform1i(glGetUniformLocation(_shaderProgramID, "_LightDynamicShadowMaps"), 3);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_StaticShadowMapCubeTextureDepth);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_DynamicShadowMapCubeTextureDepth);

	err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "LightPrep-shad GL ERROR " << err << std::endl;
	}
}

glm::vec2 Scene::getRoomsPerMap() const {
	return m_RoomsPerMap;
}

int Scene::getLightCount() const {
	//return m_LightCount;
	return m_Lights.size();
}

std::vector<LightComponent*> Scene::getLights() const {
	return m_Lights;
}

PlayerComponent* Scene::getPlayer() const {
	return m_Player;
}


void Scene::generateMap(const glm::vec2 mapSize) {
	std::cout << "... Generating Scene of size " << mapSize.x << ", " << mapSize.y << std::endl;

	WorldGenerator::generateWorld(mapSize, *this); 
}

bool Scene::collidesWithEnemies(Collider& checkFor, EnemyComponent& colliderHit) const {
	// Enemies
	for (int i = 0; i < m_Enemies.size(); i++) {
		CircleCollider enemyCollider = m_Enemies[i]->getGameObject()->getComponent<CircleColliderComponent>()->getCollider();
		if (enemyCollider.collidesWith(checkFor)) {
			colliderHit = (*m_Enemies[i]);
			return true;
		}
	}
	return false;
}

bool Scene::collidesWithPlayer(Collider& checkFor) const {
	return (m_Player->getGameObject()->getComponent<CircleColliderComponent>()->getCollider().collidesWith(checkFor));
}

bool Scene::collidesWithSceneGeometry(CircleCollider& checkFor) const {
	const glm::vec3 checkPos = checkFor.getTransform().getPosition();

	/* check the 9 fields next to the player */
	const int fieldX = glm::round(checkPos.x);
	const int fieldY = glm::round(checkPos.z); //+0.5

	for (int iX = fieldX - 1; iX < fieldX + 1; iX++) {
		for (int iY = fieldY - 1; iY < fieldY + 1; iY++) {
			//std::cout << "Field at " << iX << ", " << iY << std::endl;
			if (iX >= 0 && iY >= 0 && iX < m_FieldSize.x && iY < m_FieldSize.y) {
				Field& fieldToCheck = m_Fields[iX + iY * static_cast<int>(m_FieldSize.x)];
				BoxColliderComponent* bcc = fieldToCheck.getComponent<BoxColliderComponent>();
				if (bcc == nullptr) continue;

				//std::cout << "Blocked Field at " << iX << ", " << iY << ", of type " << fieldToCheck.m_FieldType.toString().c_str() << std::endl;
				BoxCollider bc = bcc->getCollider();

				if (bc.collidesWith(checkFor))
					return true;
				//if (PhysicsEngine::collides<BoxCollider&, CircleCollider&>(fieldToCheck.getComponent<BoxColliderComponent>()->getCollider(), checkFor)) {
				//	return true;
				//}
			}
			else {
				std::cout << "OOB" << std::endl;
			}
		}
	}

	//std::cout << "No collision with " << fieldX << ", " << fieldY << ", range = 1" << std::endl;

	return false;

}

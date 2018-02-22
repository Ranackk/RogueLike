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
#include "HUDPlayerHealthDisplayComponent.h"
#include <ammintrin.h>

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

	/* Setup fields */
	m_RoomsPerMap = size;
	m_FieldSize = glm::vec2(m_RoomsPerMap.x * Game::m_s_cRoomWidthInFields, m_RoomsPerMap.y * Game::m_s_cRoomHeightInFields);
	generateMap(m_RoomsPerMap); 

	/* Setup PlayerComponent */
	GameObject* gO = new GameObject("Player");
	this->m_Player = gO->addComponent(new PlayerComponent());
	this->m_Player->initialize(this);
	this->m_Lights.push_back(m_Player->m_Light);

	/* Shadow Mapping */
	m_DepthMaterial = Game::getInstance()->getMaterialManager()->getMaterialByName("mat_TEC_RenderDistanceToNearestSurfaceIntoDepthBuffer");

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
	m_StaticRenderComponents = std::vector<RenderComponent*>();
	m_DynamicRenderComponents = std::vector<RenderComponent*>();
	m_DynamicNonShadowRenderComponents = std::vector<RenderComponent*>();
	m_HudRenderComponents = std::vector<HUDRenderComponent*>();

	/* === Static Rendering === */
	/* Map */
	for (unsigned int i = 0; i < m_FieldBatches.size(); i++) {
		m_StaticRenderComponents.push_back(m_FieldBatches[i].getComponent<RenderComponent>());
	}

	/* === Dynamic Rendering === */
	/* Enemies */
	// Sort Enemies by type
	std::map<EnemyComponent::EnemyType, std::vector<GameObject*>> enemiesByType = std::map<EnemyComponent::EnemyType, std::vector<GameObject*>>();
	for (int i = 0; i < m_Enemies.size(); i++) {
		EnemyComponent::EnemyType type = m_Enemies[i]->getType();
		const auto res = m_EnemyPools.find(type);
		if (res != m_EnemyPools.end()) {
			enemiesByType[type].push_back(m_Enemies[i]->getGameObject());
			enemiesByType[type] = std::vector<GameObject*>();
		}
		else enemiesByType[type].push_back(m_Enemies[i]->getGameObject());
	}
	// Create the pools out of the lists
	m_EnemyPools = std::map<EnemyComponent::EnemyType, GameObjectPool>();
	for (auto it = enemiesByType.begin(); it != enemiesByType.end(); ++it) {
		GameObjectPool enemyPool = GameObjectPool();
		RenderComponent* rc = it->second[0]->getComponent<RenderComponent>();
		enemyPool.initialize(64, rc->getModelData(), rc->getMaterial());
		enemyPool.initWithGameObjectVector(it->second);
		enemyPool.updateRenderBatch();
		m_EnemyPools[it->first] = enemyPool;
		m_DynamicRenderComponents.push_back(m_EnemyPools[it->first].getRenderBatch().getComponent<RenderComponent>());
	}
	
	/* Projectiles */
	m_ProjectilePool = GameObjectPool();
	std::shared_ptr<Material> mat = Game::getInstance()->getMaterialManager()->getMaterialByName("mat_Projectile");
	mat->setTexture(Game::getInstance()->getTextureManager()->getTextureByIdentifier("tex_Player"));
	m_ProjectilePool.initialize(128, Game::getInstance()->getModelManager()->getModelDataByIdentifier("mesh_Projectile"), mat);
	m_DynamicNonShadowRenderComponents.push_back(m_ProjectilePool.getRenderBatch().getComponent<RenderComponent>());

	/* Player */
	m_DynamicRenderComponents.push_back(m_Player->getGameObject()->getComponent<RenderComponent>());

	/* === HUD Rendering === */
	m_HUDHealthContainer = new GameObject("HUD_HeartDisplay_Main");
	HUDPlayerHealthDisplayComponent* hudphdc = m_HUDHealthContainer->addComponent(new HUDPlayerHealthDisplayComponent);
	hudphdc->initialize(m_Player->getGameObject()->getComponent<HealthComponent>());
	std::vector<HUDRenderComponent*> comps = hudphdc->getHUDComponents();
	for (int i = 0; i < comps.size(); i++) {
		m_HudRenderComponents.push_back(comps[i]);
	}
}

void Scene::update(GLFWwindow* window, const float deltaTime) {
	m_Player->getGameObject()->update(window, deltaTime);

	for (auto it = m_Lights.begin(); it < m_Lights.end(); ++it) {
		(*it)->getGameObject()->update(window, deltaTime);
	}

	for (auto it = m_EnemyPools.begin(); it != m_EnemyPools.end(); ++it) {
		it->second.update(window, deltaTime);
	}
	//for (int i = 0; i < m_EnemyPools.size(); i++) {
	//	m_EnemyPools[i].update(window, deltaTime);
	//}
	m_HUDHealthContainer->update(window, deltaTime);

	m_ProjectilePool.update(window, deltaTime);

	for (int i = 0; i < m_FieldSize.x * m_FieldSize.y; i++) {
		m_Fields[i].update(window, deltaTime);
	}
}

/* Draws the whole scene to the screen */
void Scene::drawFull(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial) {

	/* Draw static geometry */
	drawStaticShadowCasters(_perspectiveMatrix, _viewMatrix, customMaterial);

	/* Draw dynamic geometry */
	drawDynamicShadowCasters(_perspectiveMatrix, _viewMatrix, customMaterial);
	drawDynamicNonShadowCasters(_perspectiveMatrix, _viewMatrix, customMaterial);

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

void Scene::drawDynamicNonShadowCasters(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix,
	Material* customMaterial) {
	for (unsigned int i = 0; i < m_DynamicNonShadowRenderComponents.size(); i++) {
		m_DynamicNonShadowRenderComponents[i]->draw(_perspectiveMatrix, _viewMatrix, customMaterial);
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

glm::vec3 Scene::getStartingPoint() const
{
	glm::vec3 ret = glm::vec3(ceil(m_RoomsPerMap.x / 2) * (Game::m_s_cRoomWidthInFields) + 0.5 * Game::m_s_cRoomWidthInFields, 0, ceil(m_RoomsPerMap.y / 2) * (Game::m_s_cRoomHeightInFields) + 0.5 * Game::m_s_cRoomHeightInFields);
	return ret;
}

glm::vec2 Scene::getCurrentRoomGridPos() const {
	const glm::vec3 pPos = m_Player->getGameObject()->getTransform().getPosition();
	return glm::vec2(floor(round(pPos.x) / Game::m_s_cRoomWidthInFields),
		floor(round(pPos.z) / Game::m_s_cRoomHeightInFields));
}

glm::vec2 Scene::getRoomGridPos(const glm::vec3 _position) const {
	return glm::vec2(floor(round(_position.x) / Game::m_s_cRoomWidthInFields),
		floor(round(_position.z) / Game::m_s_cRoomHeightInFields));
}

glm::vec3 Scene::getCurrentRoomMid() const {
	const glm::vec2 roomPos = getCurrentRoomGridPos();
	return glm::vec3((roomPos.x + 0.5) * Game::m_s_cRoomWidthInFields, 0, (roomPos.y + 0.5) * Game::m_s_cRoomHeightInFields);
}

PlayerComponent* Scene::getPlayer() const {
	return m_Player;
}


void Scene::generateMap(const glm::vec2 mapSize) {
	std::cout << "... Generating Scene of size " << mapSize.x << ", " << mapSize.y << std::endl;

	WorldGenerator::generateWorld(mapSize, *this); 
}

bool Scene::collidesWithEnemies(Collider& checkFor, EnemyComponent*& colliderHit) const {
	// Enemies
	for (int i = 0; i < m_Enemies.size(); i++) {
		CircleCollider enemyCollider = m_Enemies[i]->getGameObject()->getComponent<CircleColliderComponent>()->getCollider();
		if (enemyCollider.collidesWith(checkFor)) {
			colliderHit = m_Enemies[i];
			return true;
		}
	}
	return false;
}

bool Scene::collidesWithPlayer(Collider& checkFor) const {
	return (m_Player->getGameObject()->getComponent<CircleColliderComponent>()->getCollider().collidesWith(checkFor));
}

bool Scene::collidesWithSceneGeometry(CircleCollider& checkFor, const bool _outOfBoundsIsCollision) const {
	const glm::vec3 checkPos = checkFor.getTransform().getPosition();

	/* check the 9 fields next to the player */
	const int fieldX = glm::round(checkPos.x);
	const int fieldY = glm::round(checkPos.z); //+0.5

	for (int iX = fieldX - 1; iX < fieldX + 1; iX++) {
		for (int iY = fieldY - 1; iY < fieldY + 1; iY++) {
			//std::cout << "FieldComponent at " << iX << ", " << iY << std::endl;
			if (iX >= 0 && iY >= 0 && iX < m_FieldSize.x && iY < m_FieldSize.y) {
				FieldComponent& fieldToCheck = m_Fields[iX + iY * static_cast<int>(m_FieldSize.x)];
				BoxColliderComponent* bcc = fieldToCheck.getGameObject()->getComponent<BoxColliderComponent>();
				if (bcc == nullptr) continue;

				//std::cout << "Blocked FieldComponent at " << iX << ", " << iY << ", of type " << fieldToCheck.m_FieldType.toString().c_str() << std::endl;
				BoxCollider bc = bcc->getCollider();

				if (bc.collidesWith(checkFor))
					return true;
				//if (PhysicsEngine::collides<BoxCollider&, CircleCollider&>(fieldToCheck.getComponent<BoxColliderComponent>()->getCollider(), checkFor)) {
				//	return true;
				//}
			}
			else {
				if (_outOfBoundsIsCollision) return true;
				//std::cout << "OOB" << std::endl;
			}
		}
	}

	//std::cout << "No collision with " << fieldX << ", " << fieldY << ", range = 1" << std::endl;

	return false;

}


bool Scene::enemyInRoom(const glm::vec2 _roomCoord) {
	// TODO: find out if an enemy is in this room!
	//std::cout << "Implement this function pls " << std::endl;

	for (int i = 0; i < m_Enemies.size(); i++) {
		if (m_Enemies[i]->getRoomCoord() == _roomCoord) return true;
	}

	return false;
}
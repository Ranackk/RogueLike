#pragma once
#include "stdafx.h"
#include "FieldComponent.h"
#include "RenderBatch.h"
#include "LightComponent.h"
#include "PlayerComponent.h"
#include "Collider.h"
#include "EnemyComponent.h"
#include "HUDRenderComponent.h"
#include "GameObjectPool.h"

class CircleCollider;

class Scene
{
public:
	Scene();
	explicit Scene(glm::vec2 size);
	~Scene();

	void setupSystems();

	void update(GLFWwindow* window, const float deltaTime);

	void drawFull(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial = nullptr);
	void drawStaticShadowCasters(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial);
	void drawDynamicShadowCasters(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial);
	void drawDynamicNonShadowCasters(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial);
	void drawHudElements(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial);

	void drawStaticShadowMaps();
	void drawDynamicShadowMaps();

	void prepareLightsForShaderProgram(const GLuint _shaderProgramID) const;

	class PlayerComponent* getPlayer() const;

	bool collidesWithSceneGeometry(CircleCollider &checkFor, bool _outOfBoundsIsCollision = false) const;
	bool collidesWithEnemies(Collider& checkFor, EnemyComponent*& colliderHit) const;
	bool collidesWithPlayer(Collider& checkFor) const;

	glm::vec2 getRoomsPerMap() const;
	int getLightCount() const;
	std::vector<class LightComponent*> getLights() const;

	glm::vec3 getStartingPoint() const;
	glm::vec2 getCurrentRoomGridPos() const;
	glm::vec2 getRoomGridPos(glm::vec3 _position) const;
	glm::vec3 getCurrentRoomMid() const;
	bool isPlayerFullyInRoom(glm::vec2 roomGridCoords) const;
	bool isEnemyInRoomCoord(glm::vec2 _roomCoord);

	void blockDoor(FieldType _fieldType, glm::vec2 _worldGridPosition);
	void unblockDoor(FieldType _fieldType);

	class PlayerComponent* m_Player;
	FieldComponent* m_Fields;
	std::vector<RenderBatch> m_FieldBatches;

	GLuint m_DynamicShadowMapFBO;
	GLuint m_DynamicShadowMapCubeTextureDepth;
	GLuint m_StaticShadowMapFBO;
	GLuint m_StaticShadowMapCubeTextureDepth;
	std::shared_ptr<Material> m_DepthMaterial;

	std::vector<LightComponent*> m_Lights;
	std::vector<EnemyComponent*> m_Enemies;		// Only used for creation, then forgotten
	std::map<EnemyComponent::EnemyType, GameObjectPool> m_EnemyPools;
	GameObjectPool m_ProjectilePool;


	/* Systems */ // TODO: Move the static systems to arrays for cache line optimization
	std::vector<RenderComponent*> m_StaticRenderComponents;
	std::vector<RenderComponent*> m_DynamicRenderComponents;
	std::vector<RenderComponent*> m_DynamicNonShadowRenderComponents;
	std::vector<HUDRenderComponent*> m_HudRenderComponents;
private:
	glm::vec2 m_RoomsPerMap;
	glm::vec2 m_FieldSize;

	GLuint m_LightShaderProgrammID;

	void generateMap(const glm::vec2 mapSize);

	class GameObject* m_HUDHealthContainer;

	class std::vector<GameObject*> m_BlockadeObjects;

};


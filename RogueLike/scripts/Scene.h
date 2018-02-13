#pragma once
#include "stdafx.h"
#include "Field.h"
#include "RenderBatch.h"
#include "Light.h"
#include "Player.h"
#include "Collider.h"
#include "Enemy.h"
#include "HUDRenderComponent.h"

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
	void drawHudElements(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial);

	void drawStaticShadowMaps();
	void drawDynamicShadowMaps();

	void prepareLightsForShaderProgram(const GLuint _shaderProgramID) const;

	class Player* getPlayer() const;

	bool collidesWithSceneGeometry(CircleCollider &checkFor) const;

	glm::vec2 getRoomsPerMap() const;
	int getLightCount() const;
	std::vector<class Light*> getLights() const;

	class Player* m_Player;
	Field* m_Fields;
	std::vector<RenderBatch> m_FieldBatches;

	GLuint m_DynamicShadowMapFBO;
	GLuint m_DynamicShadowMapCubeTextureDepth;
	GLuint m_StaticShadowMapFBO;
	GLuint m_StaticShadowMapCubeTextureDepth;
	std::shared_ptr<Material> m_DepthMaterial;

	std::vector<Light*> m_Lights;
	std::vector<Enemy*> m_Enemies;


	/* Systems */ // TODO: Move the static systems to arrays for cache line optimization
	std::vector<RenderComponent*> m_StaticRenderComponents;
	std::vector<RenderComponent*> m_DynamicRenderComponents;
	std::vector<HUDRenderComponent*> m_HudRenderComponents;
private:
	glm::vec2 m_RoomsPerMap;
	glm::vec2 m_FieldSize;

	GLuint m_LightShaderProgrammID;

	void generateMap(const glm::vec2 mapSize);

	class GameObject* m_TestUI;
};


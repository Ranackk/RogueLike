#pragma once
#include "GameObject.h"
#include "RenderBatch.h"

class GameObjectPool :
	public GameObject
{
public:
	GameObjectPool();
	void initialize(const int _size, const std::shared_ptr<ModelData> _modelDataToUse,
		const std::shared_ptr<Material> _materialToUse);
	void initWithGameObjectVector(std::vector<GameObject*> _vector);

	void update(GLFWwindow* window, const float deltaTime) override;

	RenderBatch& getRenderBatch();
	GameObject* getNextFreeObject(const bool updateBatch);

	void freeObject(GameObject* gameObject);
	void updateRenderBatch();

private:
	RenderBatch m_RenderBatch;

	int m_Size;
	std::vector<GameObject*> m_GameObjects;
	bool* m_InUse;

};


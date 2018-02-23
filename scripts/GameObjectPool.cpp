#include "stdafx.h"
#include "GameObjectPool.h"
#include <allocators>


GameObjectPool::GameObjectPool()
{
}

void GameObjectPool::initialize(const int _size, const std::shared_ptr<ModelData> _modelDataToUse,
	const std::shared_ptr<Material> _materialToUse) {
	m_Size = _size;
	m_GameObjects = std::vector<GameObject*>(_size);
	m_InUse = new bool[_size];

	for (int i = 0; i < _size; i++) {
		m_InUse[i] = false;
		m_GameObjects[i] = new GameObject("Pooled Object");
	}

	m_RenderBatch = RenderBatch();
	m_RenderBatch.initialize(_modelDataToUse, _materialToUse, std::vector<GameObject*>());
}

void GameObjectPool::initWithGameObjectVector(std::vector<GameObject*> _vector) {
	for (int i = 0; i < _vector.size(); i++) {
		m_GameObjects[i] = _vector[i];
		m_InUse[i] = true;
	}
}

void GameObjectPool::update(GLFWwindow* window, const float deltaTime) {
	//for (int i = 0; i < m_Size; i++) {
	//	if (m_InUse[i])
	//		m_GameObjects[i]->update(window, deltaTime);
	//}
	m_RenderBatch.update(window, deltaTime);
}

RenderBatch& GameObjectPool::getRenderBatch() {
	return m_RenderBatch;
}

GameObject* GameObjectPool::getNextFreeObject() {
	for (int i = 0; i < m_Size; i++) {
		if (m_InUse[i] == false) {
			m_InUse[i] = true;
			return m_GameObjects[i];
		}
	}

	std::cout << "ERROR: No free object in object pool available - Using object already in use!" << std::endl;
	return m_GameObjects[0];
}

void GameObjectPool::freeObjectIntoPool(GameObject* gameObject) {
	for (int i = 0; i < m_Size; i++) {
		if (m_GameObjects[i] == gameObject) {
			m_InUse[i] = false;
			updateRenderBatch();
		}
	}
}

void GameObjectPool::updateRenderBatch() {
	std::vector<GameObject*> toBatch;
	for (int i = 0; i < m_Size; i++) {
		if (m_InUse[i]) {
			toBatch.push_back(m_GameObjects[i]);
		}
	}
	m_RenderBatch.updateBatch(toBatch);
}

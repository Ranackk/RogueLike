#pragma once
#include "ModelData.h"
#include "Field.h"
#include "RenderComponent.h"

//class RenderBatch : protected RenderGameObject
//{
//public:
//	/* Raw Data */
//	std::shared_ptr<ModelData> m_ModelData;
//	std::shared_ptr<Material> m_Material;
//	 
//	/* Processsed Data */
//	glm::mat4* m_MatrixBuffer;
//	int m_BatchedModels;
//
//	/* Open GL Stuff */
//	GLuint m_MatrixBufferID;
//
//	GLuint m_MatrixAttributeID;
//
//	RenderBatch();
//	~RenderBatch();
//
//
//	void draw(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial) override;
//
//	void initialize(const std::shared_ptr<ModelData> _modelDataToUse, const std::shared_ptr<Material> _materialToUse,
//		std::vector<RenderGameObject> _renderGameObjectsWithSameModelAndMaterial);
//};

class RenderBatch : public GameObject {
public:
	RenderBatch();

	void initialize(const std::shared_ptr<ModelData> _modelDataToUse,
		const std::shared_ptr<Material> _materialToUse, std::vector<GameObject> _objectsToBatch);

	void update(GLFWwindow* window, const float deltaTime) override {}
};
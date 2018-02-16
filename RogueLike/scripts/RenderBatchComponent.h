#pragma once
#include "RenderComponent.h"

class RenderBatchComponent : public RenderComponent
{
protected:
	/* Processsed Data */
	glm::mat4* m_MatrixBuffer = nullptr;
	int m_BatchedModels = 0;

	/* Open GL Stuff */
	GLuint m_MatrixBufferID = -1;
	GLuint m_MatrixAttributeID = -1;

public:
	void draw(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial) override;
	void initialize(const std::shared_ptr<ModelData> _modelDataToUse, const std::shared_ptr<Material> _materialToUse,
		std::vector<GameObject> _ObjectsToBatch);
	void updateBatch(glm::mat4x4* _matrixBuffer, int size);
};


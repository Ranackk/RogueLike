#include "stdafx.h"
#include "RenderBatchComponent.h"


void RenderBatchComponent::draw(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix,
	Material* customMaterial) {

	/* Prepare */
	if (customMaterial == nullptr) {
		m_Material->bindMaterial(_perspectiveMatrix, _viewMatrix, glm::mat4(0),
			m_ModelData.get()->m_VertexPositionBufferID, m_ModelData.get()->m_VertexUVBufferID, m_ModelData.get()->m_VertexNormalBufferID, true);
		m_MatrixAttributeID = glGetAttribLocation(m_Material.get()->getShaderProgramId(), "_InstancedModelMatrix");		
		//std::cout << m_MatrixAttributeID << std::endl;
	}
	else {
		customMaterial->bindMaterial(_perspectiveMatrix, _viewMatrix, glm::mat4(0),
			m_ModelData.get()->m_VertexPositionBufferID, m_ModelData.get()->m_VertexUVBufferID, m_ModelData.get()->m_VertexNormalBufferID, true);
		m_MatrixAttributeID = glGetAttribLocation(customMaterial->getShaderProgramId(), "_InstancedModelMatrix");
		//std::cout << m_MatrixAttributeID << std::endl;
	}
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "MatStart GL ERROR " << err << std::endl;
	}
	/* Draw */

	// Enable the attribute arrays for the modelMatrix
	glEnableVertexAttribArray(m_MatrixAttributeID + 0);
	glEnableVertexAttribArray(m_MatrixAttributeID + 1);
	glEnableVertexAttribArray(m_MatrixAttributeID + 2);
	glEnableVertexAttribArray(m_MatrixAttributeID + 3);

	// Bind the matrix buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_MatrixBufferID);

	glVertexAttribPointer(m_MatrixAttributeID + 0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
	glVertexAttribPointer(m_MatrixAttributeID + 1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(m_MatrixAttributeID + 2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(m_MatrixAttributeID + 3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));

	// Set which attributes stay the same after each model and which differ
	glVertexAttribDivisor(0, 0);							// vertexPositions		same for all models		| 0 = no change
	glVertexAttribDivisor(1, 0);							// vertexUVs			same for all models		| 0 = no change
	glVertexAttribDivisor(2, 0);							// vertexNormals		same for all models		| 0 = no change
	glVertexAttribDivisor(m_MatrixAttributeID + 0, 1);		// matrix information	differes between models	| 1 = change after each model
	glVertexAttribDivisor(m_MatrixAttributeID + 1, 1);
	glVertexAttribDivisor(m_MatrixAttributeID + 2, 1);
	glVertexAttribDivisor(m_MatrixAttributeID + 3, 1);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "MatStart GL ERROR " << err << std::endl;
	}

	// Bind Index Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ModelData.get()->m_IndicesBufferID);

	// Draw
	glDrawElementsInstanced(GL_TRIANGLES, m_ModelData.get()->m_IndicesBufferData.size(), GL_UNSIGNED_SHORT, (void*)0, m_BatchedModels);

	//Disable the attribute arrays for the modelMatrix
	glDisableVertexAttribArray(m_MatrixAttributeID + 0);
	glDisableVertexAttribArray(m_MatrixAttributeID + 1);
	glDisableVertexAttribArray(m_MatrixAttributeID + 2);
	glDisableVertexAttribArray(m_MatrixAttributeID + 3);

	// Unbind 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Unbind */
	if (customMaterial == nullptr) {
		m_Material->unbindMaterial();
	}
	else {
		customMaterial->unbindMaterial();
	}
}

void RenderBatchComponent::initialize(const std::shared_ptr<ModelData> _modelDataToUse,
	const std::shared_ptr<Material> _materialToUse, std::vector<GameObject*> _objectsToBatch) {
	/* Save Data */
	this->m_Material = _materialToUse;
	this->m_ModelData = _modelDataToUse;
	this->m_MatrixBuffer = new glm::mat4[_objectsToBatch.size()];
	for (int i = 0; i < (int) _objectsToBatch.size(); i++) {
		_objectsToBatch[i]->getTransform().setDirty();
		_objectsToBatch[i]->getTransform().updateCurrentMatrix();
		m_MatrixBuffer[i] = _objectsToBatch[i]->getTransform().getObjectMatrix();
	}
	m_BatchedModels = _objectsToBatch.size();


	/* Create Buffer for Matrices*/
	glGenBuffers(1, &this->m_MatrixBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_MatrixBufferID);

	/* Buffer the matrix data */
	glBufferData(GL_ARRAY_BUFFER, m_BatchedModels * 16 * sizeof(GLfloat), &m_MatrixBuffer[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Fetch Attribute location of the modelmatrix */
	m_MatrixAttributeID = glGetAttribLocation(m_Material.get()->getShaderProgramId(), "_InstancedModelMatrix");
}

void RenderBatchComponent::updateBatch(glm::mat4x4* _matrixBuffer, const int size) {
	m_MatrixBuffer = _matrixBuffer;
	m_BatchedModels = size;

	/* Buffer the matrix data */
	glBindBuffer(GL_ARRAY_BUFFER, this->m_MatrixBufferID);
	glBufferData(GL_ARRAY_BUFFER, m_BatchedModels * 16 * sizeof(GLfloat), &m_MatrixBuffer[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
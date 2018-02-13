#include "stdafx.h"
#include "HUDRenderComponent.h"
#include "Game.h"


void HUDRenderComponent::draw(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix,
	Material* customMaterial) {
	
	Material* useMaterial = customMaterial;
	if (useMaterial == nullptr) useMaterial = customMaterial;
#
	/* Prepare */
	useMaterial->bindMaterial(_perspectiveMatrix, _viewMatrix, m_GameObject->getTransform().getObjectMatrix(),
			m_ModelData.get()->m_VertexPositionBufferID, m_ModelData.get()->m_VertexUVBufferID, m_ModelData.get()->m_VertexNormalBufferID, true);
	
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "HUD_RenderComponent GL ERROR " << err << std::endl;
	}
	/* Draw */


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

void HUDRenderComponent::initialize(const std::shared_ptr<Material> _material, const glm::vec2 _positionTopLeft, const glm::vec2 _dimensions) {
	m_HUDTransform_Dimensions = _dimensions;
	m_HUDTransform_TopLeft = _positionTopLeft;

	this->m_Material = _material;

	this->m_ModelData = Game::getInstance()->getModelManager()->getModelDataByPrimitve(Primitives::QUAD);
}

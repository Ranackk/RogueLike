#include "stdafx.h"
#include "HUDRenderComponent.h"
#include "Game.h"


// TODO: GENERATE CUSTOM OBJECT MATRIX
void HUDRenderComponent::draw(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial) {
	Material* useMaterial = m_Material.get();
	if (useMaterial == nullptr) useMaterial = customMaterial;
	/* Bind */
	useMaterial->bindMaterial(_perspectiveMatrix, _viewMatrix, m_GameObject->getTransform().getObjectMatrix(), m_ModelData.get()->m_VertexPositionBufferID, m_ModelData.get()->m_VertexUVBufferID, m_ModelData.get()->m_VertexNormalBufferID, true);
	/* Draw */
	this->m_ModelData.get()->draw();
	/* Unbind */
	useMaterial->unbindMaterial();
}
// WARNING: Call this method AFTER adding the component to a game object!
void HUDRenderComponent::initialize(const std::shared_ptr<Material> _material, const glm::vec2 _positionTopLeft, const glm::vec2 _dimensions) {
	m_HUDTransform_Percentage_Dimensions = _dimensions;
	m_HUDTransform_Percentage_BottomLeft = _positionTopLeft;

	this->m_Material = _material;
	this->m_GameObject->getTransform().setLocalPosition(glm::vec3(-1 + 2 *_positionTopLeft.x , -1 + 2 * _positionTopLeft.y, 0));
	this->m_GameObject->getTransform().setLocalScale(glm::vec3(_dimensions.x * 2, _dimensions.y * 2, 1));

	this->m_ModelData = Game::getInstance()->getModelManager()->getModelDataByPrimitve(Primitives::QUAD);
}

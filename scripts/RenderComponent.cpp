#include "stdafx.h"
#include "RenderComponent.h"

void RenderComponent::initialize(const std::shared_ptr<ModelData> _modelData, const std::shared_ptr<Material> _material) {
	this->m_Material = _material;
	this->m_ModelData = _modelData;
}

void RenderComponent::draw(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix,
	Material* customMaterial) {
	if (customMaterial == nullptr) {
		this->m_Material.get()->bindMaterial(_perspectiveMatrix, _viewMatrix, m_GameObject->getTransform().getObjectMatrix(), m_ModelData.get()->m_VertexPositionBufferID, m_ModelData.get()->m_VertexUVBufferID, m_ModelData.get()->m_VertexNormalBufferID, false);
		this->m_ModelData.get()->draw();
		this->m_Material.get()->unbindMaterial();
	}
	else {
		customMaterial->bindMaterial(_perspectiveMatrix, _viewMatrix, m_GameObject->getTransform().getObjectMatrix(), m_ModelData.get()->m_VertexPositionBufferID, m_ModelData.get()->m_VertexUVBufferID, m_ModelData.get()->m_VertexNormalBufferID, false);
		this->m_ModelData.get()->draw();
		customMaterial->unbindMaterial();
	}
}

GLuint RenderComponent::getSprite() const {
	return m_SpriteID;
}

void RenderComponent::setSprite(const GLuint sprite) {
	this->m_SpriteID = sprite;
}

std::shared_ptr<ModelData> RenderComponent::getModelData() const {
	return m_ModelData;
}

std::shared_ptr<Material> RenderComponent::getMaterial() const {
	return m_Material;
}

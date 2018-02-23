#pragma once
#include "Component.h"
#include "Material.h"
#include "ModelData.h"

class RenderComponent : public Component
{
protected:
	GLuint m_SpriteID = -1;

	std::shared_ptr<ModelData> m_ModelData;
	std::shared_ptr<Material> m_Material;
public:
	void initialize(std::shared_ptr<ModelData> _modelData, std::shared_ptr<Material> material);
	virtual void draw(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial = nullptr);

	GLuint getSprite() const;
	void setSprite(const GLuint sprite);
	std::shared_ptr<ModelData> getModelData() const;
	std::shared_ptr<Material> getMaterial() const;

};


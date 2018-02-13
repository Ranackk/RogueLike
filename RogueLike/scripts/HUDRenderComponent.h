#pragma once
#include "RenderComponent.h"

/*
 * Used for rendering a HUD Element.
 * This render component does not use the transforms position but its own values that are given in screen percentage.
 */
class HUDRenderComponent : public RenderComponent
{
protected:
	glm::vec2 m_HUDTransform_Percentage_BottomLeft;
	glm::vec2 m_HUDTransform_Percentage_Dimensions;

public:
	void draw(const glm::mat4x4 _perspectiveMatrix, const glm::mat4x4 _viewMatrix, Material* customMaterial) override;
	void initialize(const std::shared_ptr<Material> _material, const glm::vec2 _percentageBottomLeft, const glm::vec2 _percentageDimensions);
};


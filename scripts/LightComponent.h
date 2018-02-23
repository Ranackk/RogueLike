#pragma once
#include "GameObject.h"
#include "Material.h"
#include "Scene.h"

class LightComponent : public UpdateComponent
{
public:
	enum LightStasis {
		STATIC, DYNAMIC
	};

private:
	int m_UUID;

	/* Properties */
	LightStasis m_Static;
	float m_LightRange;
	float m_DefaultLightRange;
	glm::vec4 m_LightColor;

	glm::mat4x4 m_CubeMapViewMatrices[6];

	float m_FlickerCounter = 0;
	bool m_Flicker;
public:


	static int m_s_LightCount;

	LightComponent();

	void initialize(const glm::vec3 _lightPosition, const float _lightRange, const glm::vec4 _lightColor, const LightStasis _isStatic, const bool _flicker);
	bool prepareForRender(float &range, glm::vec3 &position, glm::vec4 &color);
	void update(GLFWwindow* window, const float deltaTime) override;
	glm::mat4x4* getCubeMapViewMatrices();

	bool isStatic() const { return m_Static == STATIC; }


};


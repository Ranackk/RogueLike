#include "stdafx.h"
#include "LightComponent.h"
#include "TextureManager.h"
#include "RenderEngine.h"
#include "Game.h"
#include <detail/_vectorize.hpp>
#include <detail/_vectorize.hpp>
#include <detail/_vectorize.hpp>
#include <detail/_vectorize.hpp>
#include <gtc/matrix_transform.inl>


int LightComponent::m_s_LightCount = 0;

// Doesnt initialize the LightComponent yet
LightComponent::LightComponent()
{
	m_UUID = m_s_LightCount++;
	m_LightRange = 0;
	m_DefaultLightRange = 0;
}

void LightComponent::initialize(const glm::vec3 _lightPosition, const float _lightRange, const glm::vec4 _lightColor,
	const LightStasis _isStatic, const bool _flicker) {

	m_UUID = m_s_LightCount++;
	m_DefaultLightRange = _lightRange;
	m_LightRange = _lightRange;
	m_LightColor = _lightColor;
	m_GameObject->getTransform().setLocalPosition(_lightPosition);
	m_Static = _isStatic;
	m_Flicker = _flicker;

	m_FlickerCounter = rand();

	for (int i = 0; i < 6; i++) {
		m_CubeMapViewMatrices[i] = glm::lookAt(_lightPosition, _lightPosition + RenderEngine::getCubeMapFaceDirection(i), RenderEngine::getCubeMapUpVector(i));
	}
}


bool LightComponent::prepareForRender(float &range, glm::vec3 &position, glm::vec4 &color) {
	// TODO: IMPLEMENT A CHECK IF THE LIGHT IS IN RANGE OF CAMERA

	const glm::vec3 lightPosition = m_GameObject->getTransform().getPosition();
	/* Set the output variables */
	range = m_LightRange;
	position = lightPosition;
	color = m_LightColor;

	return true;
}

void LightComponent::update(GLFWwindow * window, const float deltaTime)
{
	if (m_Flicker) {
		m_FlickerCounter += deltaTime;
		m_LightRange = m_DefaultLightRange + (m_DefaultLightRange * 0.3f) * glm::sin(m_FlickerCounter * 1.2);
	}
}

glm::mat4x4* LightComponent::getCubeMapViewMatrices() {
	if (m_Static == DYNAMIC) {
		const glm::vec3 pos = m_GameObject->getTransform().getPosition();
		for (int i = 0; i < 6; i++) {
			m_CubeMapViewMatrices[i] = glm::lookAt(pos, pos + RenderEngine::getCubeMapFaceDirection(i), RenderEngine::getCubeMapUpVector(i));
		}
	}
	return m_CubeMapViewMatrices;
}

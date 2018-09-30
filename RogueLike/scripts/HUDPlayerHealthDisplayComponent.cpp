#include "stdafx.h"
#include "HUDPlayerHealthDisplayComponent.h"
#include <string>
#include "Game.h"


void HUDPlayerHealthDisplayComponent::initialize(HealthComponent* _healthComponentToDisplay) {
	m_HealthComponent				= _healthComponentToDisplay;
	m_CurrentHeartContainerCount	= (int) ceil(*m_HealthComponent->getMaximumHealthPointer());
	m_CurrentVisualHP				= *m_HealthComponent->getCurrentHealthPointer();
	createHudDisplay();
}

void HUDPlayerHealthDisplayComponent::update(GLFWwindow* window, const float deltaTime) {
	const float currentHp = *m_HealthComponent->getCurrentHealthPointer();
	if (currentHp != m_CurrentVisualHP) {
		m_CurrentVisualHP -= deltaTime;
		if (m_CurrentVisualHP < currentHp) m_CurrentVisualHP = currentHp;
	}

	/* Let last heart blink */
	std::shared_ptr<Material> uiMat = Game::getInstance()->getMaterialManager()->getMaterialByName("mat_UIHeartContainer0");
	if (m_CurrentVisualHP <= 1.0f) {
		uiMat->setBlink(1);
	}
	else {
		uiMat->setBlink(0);
	}

	/* Fill all hearts to their value */
	float amountToFill = m_CurrentVisualHP;

	for (int i = 0; i != m_CurrentHeartContainerCount; i++) {
		if (amountToFill >= 1.0f) {
			m_FillAmountsVisual[i] = 1.0f;
			amountToFill--;
		}
		else {
			m_FillAmountsVisual[i] = amountToFill;
			amountToFill = 0;
		}
	}
}

void HUDPlayerHealthDisplayComponent::createHudDisplay() {
	m_HeartContainers = std::vector<HUDRenderComponent*>(m_CurrentHeartContainerCount);
	m_FillAmountsVisual = std::vector<float>(m_CurrentHeartContainerCount);
	for (int i = 0; i != m_CurrentHeartContainerCount; i++) {
		std::shared_ptr<Material> uiMat = Game::getInstance()->getMaterialManager()->getMaterialByName("mat_UIHeartContainer" + std::to_string(i));
		uiMat->setupHealthBarShader(Game::getInstance()->getTextureManager()->getTextureByIdentifier("tex_UI_HeartContainer_BG"), Game::getInstance()->getTextureManager()->getTextureByIdentifier("tex_UI_HeartContainer_Fill"),
			Game::getInstance()->getTextureManager()->getTextureByIdentifier("tex_UI_HeartContainer_Gradient"), &m_FillAmountsVisual[i]);

		GameObject* gO = new GameObject("HUD_HeartDisplay " + std::to_string(i));
		m_HeartContainers[i] = gO->addComponent(new HUDRenderComponent);
		m_HeartContainers[i]->initialize(uiMat, glm::vec2(0.015f + 0.03f * i, 1.0f - 0.08f), glm::vec2(0.03f, 0.053f));
	}
}

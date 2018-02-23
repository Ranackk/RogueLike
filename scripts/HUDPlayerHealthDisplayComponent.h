#pragma once
#include "Component.h"
#include "HUDRenderComponent.h"
#include "HealthComponent.h"

class HUDPlayerHealthDisplayComponent :
	public UpdateComponent
{
public:
	void initialize(HealthComponent* _healthComponentToDisplay) ;
	void update(GLFWwindow* window, const float deltaTime) override;

	std::vector<HUDRenderComponent*> getHUDComponents() const { return m_HeartContainers; }
private:
	HealthComponent* m_HealthComponent = nullptr;

	float m_CurrentVisualHP;
	std::vector<float> m_FillAmountsVisual;

	int m_CurrentHeartContainerCount;
	std::vector<HUDRenderComponent*> m_HeartContainers;

	void createHudDisplay();
};


#pragma once
#include "d:\Dateisystem\Games n Programs\Workspace Visual Studio\RogueLike\RogueLike\scripts\Component.h"
class HealthComponent :
	public Component
{
public:
	void initialize(float _current = -1, float _max = -1);

	/* Setters */
	bool takeDamage(float _damage);

	/* Getters */
	const float* getCurrentHealthPointer() const { return &m_CurrentHealth; }
	const float* getMaximumHealthPointer() const { return &m_MaximumHealth; }
private:
	float m_MaximumHealth = -1;
	float m_CurrentHealth = -1;
};


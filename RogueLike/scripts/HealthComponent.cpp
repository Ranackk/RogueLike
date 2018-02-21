#include "stdafx.h"
#include "HealthComponent.h"

bool HealthComponent::takeDamage(const float _damage) {
	m_CurrentHealth -= _damage;
	return !(m_CurrentHealth > 0) && m_CurrentHealth != -1;
}

void HealthComponent::initialize(const float _max) {
	m_CurrentHealth = _max;
	m_MaximumHealth = _max;
}

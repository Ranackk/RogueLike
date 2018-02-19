#include "stdafx.h"
#include "HealthComponent.h"

bool HealthComponent::takeDamage(const float _damage) {
	m_CurrentHealth -= _damage;
	return !(m_CurrentHealth > 0) && m_CurrentHealth != -1;
}

void HealthComponent::initialize(const float _current, const float _max) {
	if (_current != -1) m_CurrentHealth = _current;
	if (_max != -1) m_MaximumHealth = _max;
}

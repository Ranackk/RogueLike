#include "stdafx.h"
#include "Component.h"


Component::Component()
{

}

Component::~Component()
{
}

void Component::setGameObject(GameObject* _gameObject) {
	m_GameObject = _gameObject;
}

GameObject* Component::getGameObject() {
	return m_GameObject;
}

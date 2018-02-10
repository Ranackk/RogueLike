#pragma once
#include "stdafx.h"

class GameObject;

class Component
{
public:
	Component();
	virtual ~Component();

	void setGameObject(class GameObject* _gameObject);
	GameObject* getGameObject();
protected:
	GameObject* m_GameObject;
};


#pragma once
#include "Component.h"

class UpdateComponent :
	public Component
{
public:
	virtual void update(GLFWwindow* window, const float deltaTime);
};


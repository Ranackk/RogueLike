#pragma once
#include "Component.h"
#include "Collider.h"

class ColliderComponent : public Component {
public:
	void addCollisionHandler(void(* _func)(ColliderComponent _other));
	void fireCollision(ColliderComponent _other);
private:
	// Vector of function pointers that get called upon a collision between the colliders.
	std::vector<void(*)(ColliderComponent)> e_OnCollision;
};

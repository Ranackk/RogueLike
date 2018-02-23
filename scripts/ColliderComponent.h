#pragma once
#include "Component.h"
#include "Collider.h"

class ColliderComponent : public Component {
	/* Quick note: This on collision handler system should work with a real physics system that checks everything with everything.
	 * Due to time constraints and scope, I instead used a semi-automatic system where I check for collisions manually and then manually perform according action.
	 */
public:
	void addCollisionHandler(void(* _func)(ColliderComponent _other));
	void fireCollision(ColliderComponent _other);
private:
	// Vector of function pointers that get called upon a collision between the colliders.
	std::vector<void(*)(ColliderComponent)> e_OnCollision;
};

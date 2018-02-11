#include "stdafx.h"
#include "ColliderComponent.h"
#include <memory>
#include "Collider.h"

void ColliderComponent::addCollisionHandler(void(* _func)(ColliderComponent _other)) {
	e_OnCollision.push_back(_func);
}

void ColliderComponent::fireCollision(const ColliderComponent _other) {
	for (auto it = e_OnCollision.begin(); it != e_OnCollision.end(); ++it) {
		(*it)(_other);
	}
}

#pragma once
#include "GameObject.h"
#include <memory>

class CircleCollider;
class BoxCollider;

class Collider
{
public:
	virtual void initialize(std::shared_ptr<GameObject> _gameObject);

	Transform getTransform() const { return m_GameObject->getTransform(); }

	virtual bool collidesWith(Collider& other) = 0;
	virtual bool colldesWithCircle(CircleCollider& circleCollider) = 0;
	virtual bool collidesWithBox(BoxCollider& boxCollider) = 0;

	virtual ~Collider();
private:
	std::shared_ptr<GameObject> m_GameObject = nullptr;

};
#pragma once
#include "GameObject.h"
#include <memory>
#include "PhysicsEngine.h"

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

	void setCollisionLayer(const PhysicsEngine::CollisionLayer _layer) { m_CollisionLayer = _layer; }
	unsigned char getCollisionLayer() const { return m_CollisionLayer; }
private:
	std::shared_ptr<GameObject> m_GameObject = nullptr;

	// By default, the layer of the collider is 0, which is "not set". It can not collide with anything!
	unsigned char m_CollisionLayer = 0x00000000;

};
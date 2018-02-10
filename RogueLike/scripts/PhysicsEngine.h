#pragma once
#include "stdafx.h"
#include "BoxCollider.h"
#include "CircleCollider.h"

class PhysicsEngine
{
public:
	//template<class T, class N>
	//static bool collides(T c1, N c2)
	//{
	//	std::cerr << "Cant detect collision between given types!" << std::endl;
	//	return false;
	//}

	static bool collides(BoxCollider &c1, BoxCollider &c2);
	static bool collides(BoxCollider &c1, CircleCollider &c2);
	static bool collides(CircleCollider &c1, CircleCollider &c2);
};

	//template <>
	//inline bool PhysicsEngine::collides(BoxCollider &c1, CircleCollider &c2) {
	//	const glm::vec3 dist = glm::abs(c1.getTransform().getPosition() - c2.getTransform().getPosition());

	//	/* If the distance between box and circle is bigger than box radius & circle radius, there is no collision*/
	//	if (dist.x > c1.m_Dimensions.x / 2 + c2.m_Radius)		return false;
	//	if (dist.z > c1.m_Dimensions.y / 2 + c2.m_Radius)		return false;

	//	/* If the distance between box and circle is smaller than the box radius, there is a collision */
	//	if (dist.x <= c1.m_Dimensions.x / 2)						return true;
	//	if (dist.z <= c1.m_Dimensions.y / 2)						return true;

	//	/* Check if the circle overlaps the boxes corner */
	//	return glm::pow(dist.x - c1.m_Dimensions.x / 2, 2) + glm::pow(dist.z - c1.m_Dimensions.y / 2, 2) <= glm::pow(c2.m_Radius, 2);
	//}

	//template <>
	//inline bool PhysicsEngine::collides(CircleCollider &c1, CircleCollider &c2) {
	//	const float dist = glm::distance(c1.getTransform().getPosition(), c2.getTransform().getPosition());

	//	/* Check if the distance is smaller than the combined radius of both circles */
	//	return (dist <= c1.m_Radius + c2.m_Radius);
	//}

	//static bool collides(BoxCollider box, CircleCollider circ);
	//static bool collides(CircleCollider c1, CircleCollider c2);



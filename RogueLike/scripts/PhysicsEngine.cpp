#include "stdafx.h"
#include "PhysicsEngine.h"
#include "BoxCollider.h"
#include "CircleCollider.h"

bool PhysicsEngine::collides(BoxCollider& c1, BoxCollider& c2) {
	if (!layerMaskAllowsCollision(c1, c2)) return false;
	std::cout << "Box Box Collision not implemented yet!";
	return false;
}

 bool PhysicsEngine::collides(BoxCollider &c1, CircleCollider &c2)
{
	 if (!layerMaskAllowsCollision(c1, c2)) return false;

	const glm::vec3 dist = glm::abs((c1.getTransform().getPosition() - c1.m_MidpointOffset) - (c2.getTransform().getPosition() - c2.m_MidpointOffset));

	/* If the distance between box and circle is bigger than box radius & circle radius, there is no collision*/
	if (dist.x > c1.m_Dimensions.x / 2 + c2.m_Radius)		return false;
	if (dist.z > c1.m_Dimensions.y / 2 + c2.m_Radius)		return false;

	/* If the distance between box and circle is smaller than the box radius, there is a collision */
	if (dist.x <= c1.m_Dimensions.x / 2)						return true;
	if (dist.z <= c1.m_Dimensions.y / 2)						return true;

	/* Check if the circle overlaps the boxes corner */
	return glm::pow(dist.x - c1.m_Dimensions.x / 2, 2) + glm::pow(dist.z - c1.m_Dimensions.y / 2, 2) < glm::pow(c2.m_Radius, 2);
}
 bool PhysicsEngine::collides(CircleCollider &c1, CircleCollider &c2)
{
	 if (!layerMaskAllowsCollision(c1, c2)) return false;

	const float dist = glm::distance(c1.getTransform().getPosition(), c2.getTransform().getPosition());

	/* Check if the distance is smaller than the combined radius of both circles */
	return (dist <= c1.m_Radius + c2.m_Radius);
}

bool PhysicsEngine::layerMaskAllowsCollision(Collider& c1, Collider& c2) {
	/* Use the collision layer of both colliders to find at which index in the collision layer flag field the flag is 
	 * located that tells about whether those 2 layers do collide		
	 */
	//int n = c1.getCollisionLayer();
	//int m = c2.getCollisionLayer();
	//int n = 1;
	//int m = 2;

	///* Swap the indicies if needed */
	//if (n > m) {
	//	const int t = n;
	//	n = m;
	//	m = t;
	//}

	//const unsigned char checkFlagIndex = n * m_s_c_CollisionLayers - (pow(n,2) - n) / 2 + m - n; //n * L		- (n^2 - n) / 2		+ m - n
	///* Transform the index into a number that can be used to bitwise AND the global flag field*/
	//const unsigned char checkFlag = pow(2, checkFlagIndex);

	///* Return true if the flag field is "1" at the checkFlag, "false" if not. */
	//const unsigned char afterCheck = (m_s_c_CollisionLayerMaskFlagField & checkFlag);
	//return afterCheck == checkFlag;

	return true;
}

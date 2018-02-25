#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "BoxCollider.h"
#include "UpdateComponent.h"
#include "HealthComponent.h"

class Scene;


/**
* \brief Represents a EnemyComponent that has 1hp and deals 1 damage on player collision
*/
class EnemyComponent :
	public UpdateComponent
{
public:
	enum EnemyType {
		ROGUE, ARCHER
	};

	static EnemyType getTypeByColor(int v);

	enum Range {
		MELEE, RANGE
	};

	EnemyComponent();
	void initialize(Scene* _scene, const EnemyType _type, glm::vec3 _position);

	void update(GLFWwindow* _window, const float _deltaTime) override;

	float getDamage(Range _range) const;
	void takeDamage(const float _amount, const glm::vec3 _knockback) const;
	void die() const;
	void setRoomCoord(const glm::vec2 _roomCoord);
	glm::vec2 getRoomCoord() const { return m_RoomCoord; }
	 
	EnemyType getType() const { return m_Type; }
private:
	EnemyType m_Type;
	Scene* m_Scene;
	CircleCollider* m_CircleCollider;
	HealthComponent* m_HealthComponent;
	glm::vec2 m_RoomCoord;

	/* Enemy Type specific functions */
	void initializeEnemyTypeRogue();
	void initializeEnemyTypeArcher();
	void updateEnemyTypRogue(GLFWwindow* _window, const float _deltaTime);
	void updateEnemyTypArcher(GLFWwindow* _window, const float _deltaTime);

	float m_AliveTime = 0.0f;
	glm::vec3 m_StartPosition;
	glm::vec2 m_RoomGridPos;

	/* Enemy Type specific variables */
	/* === Rogue === */
	bool m_RogueCharging = false;
	float m_RogueSpeed = 2.0;
	float m_RogueMeleeDamage = 1.0f;
	float m_RogueHealthPoints = 3.0f;

	/* === Archer === */
	float m_ArcherShootSpeed = 4.0f;
	float m_ArcherShootCooldownDuration = 0.7f;
	float m_ArcherShootCooldown = 0.0f;
	float m_ArcherRangedDamage = 0.5f;
	float m_ArcherMeleeDamage = 1.0f;
	float m_ArcherHealthPoints = 2.0f;
	float m_ArcherAggroRangeX = 5.0f;
	float m_ArcherAggroRangeY = 4.0f;
};
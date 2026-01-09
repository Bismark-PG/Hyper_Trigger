/*==============================================================================

	Manage Enemy And Enemy Type [Enemy.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef ENEMY_H
#define ENEMY_H
#include <DirectXMath.h>
#include "Collision.h"
#include "model.h"

enum class EnemyState
{
	IDLE,
	CHASE,
	WAIT,
	RETURN
};

enum class EnemyType
{
	GROUND,
	FLIGHT 
};

class Enemy
{
public:
	Enemy(const DirectX::XMFLOAT3& pos);
	virtual ~Enemy();

	// --- Pulling System ---
	virtual void Activate(const DirectX::XMFLOAT3& pos);
	virtual void Deactivate();
	bool IsActive() const { return m_IsActive; }
	EnemyType GetType() const { return m_MyType; }

	// --- Main Logic ---
	virtual void Update(double elapsed_time);
	virtual void Draw();

	void OnDamage(int damage);
	bool IsDead() const { return m_HP <= 0; }
	AABB GetAABB() const;
	DirectX::XMFLOAT3 GetPosition() const { return Position; }

protected:
	// --- State ---
	bool m_IsActive = false;
	EnemyType m_MyType;

	// --- Stat ---
	int m_HP = 100;
	int m_MaxHP = 100;
	float m_Speed = 2.0f;

	// --- Transform ---
	DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f }; 
	DirectX::XMFLOAT3 Rotation = { 0.0f, 0.0f, 0.0f }; 
	float m_Scale = 1.0f;                                

	// --- Resource ---
	MODEL* m_pModel = nullptr;
	EnemyState m_State = EnemyState::IDLE;
	DirectX::XMFLOAT3 m_SpawnPos = {};
	float m_WaitTimer = 0.0f;
	float m_Gravity = 0.0f;
	float m_BottomOffset = 0.0f;

	// --- Physical Defense ---
	void Enemy_Collision_Map(double dt);
	void Enemy_Collision_Player(double dt);
	void Enemy_Collision_Enemy(double dt);
};
#endif // ENEMY_H
/*==============================================================================

	Manage Enemy And Enemy Type [Enemy.h]

	Author : Choi HyungJoon

==============================================================================*/
#include "Enemy.h"
#include "Enemy_Manager.h"
#include "Enemy_Ground.h"
#include "Enemy_Flight.h"
#include "Game_Model.h"
#include "Player.h"
#include "Map_System.h"
#include "Debug_Collision.h"
#include <vector>
#include <algorithm>
#include <Game.h>
using namespace DirectX;

// Enemy Class Implementation
Enemy::Enemy(const XMFLOAT3& pos)
	: Position(pos),
	m_MyType(EnemyType::GROUND)
{
	// Save Spawn Position
	m_SpawnPos = pos;
	// Default State
	m_State = EnemyState::IDLE;
	// Sample Model
	m_pModel = Model_Manager::GetInstance()->GetModel("Ball");

	if (m_pModel)
	{
		m_BottomOffset = fabs(m_pModel->Local_AABB.Min.y) * m_Scale;
	}
}

Enemy::~Enemy()
{
}

void Enemy::Activate(const XMFLOAT3& pos)
{
	m_IsActive = true;
	Position = pos;
	m_SpawnPos = pos;
	m_HP = m_MaxHP;
	m_State = EnemyState::IDLE;
	m_Gravity = 0.0f;
	m_WaitTimer = 0.0f;
}

void Enemy::Deactivate()
{
	m_IsActive = false;
	// Need Destroy Effect?
}

void Enemy::Update(double elapsed_time)
{
}

void Enemy::Draw()
{
	if (!m_pModel) return;

	XMMATRIX mtxScale = XMMatrixScaling(m_Scale, m_Scale, m_Scale);
	XMMATRIX mtxRot = XMMatrixRotationY(Rotation.y);
	XMMATRIX mtxTrans = XMMatrixTranslation(Position.x, Position.y, Position.z);

	XMMATRIX mtxWorld = mtxScale * mtxRot * mtxTrans;

	ModelDraw(m_pModel, mtxWorld);

	if (Get_Debug_Mode_State())
	{
		Debug_Collision_Draw(GetAABB(), { 1.0f, 0.0f, 0.0f, 1.0f });
	}
}

void Enemy::OnDamage(int damage)
{
	if (!m_IsActive)
	{
		return;
	}

	int FinalDamage = {};
	FinalDamage = FinalDamage - damage;

	if (FinalDamage < 1)
	{
		FinalDamage = 1;
	}

	m_HP -= FinalDamage;

	if (m_HP <= 0)
	{
		Deactivate();
	}
}

AABB Enemy::GetAABB() const
{
	if (!m_pModel) return AABB{ {0,0,0}, {0,0,0} };

	AABB worldAABB;
	XMVECTOR Min = XMLoadFloat3(&m_pModel->Local_AABB.Min) * m_Scale;
	XMVECTOR Max = XMLoadFloat3(&m_pModel->Local_AABB.Max) * m_Scale;
	XMVECTOR Pos = XMLoadFloat3(&Position);

	XMStoreFloat3(&worldAABB.Min, Min + Pos);
	XMStoreFloat3(&worldAABB.Max, Max + Pos);

	return worldAABB;
}

void Enemy::Enemy_Collision_Map(double dt)
{
	AABB E_AABB = GetAABB();
	int OBJ_Count = Map_System_Get_Object_Count();

	XMVECTOR Enemy_POS = XMLoadFloat3(&Position);

	for (int i = 0; i < OBJ_Count; i++)
	{
		const MapObject* OBJ = Map_System_Get_Object(i);

		if (OBJ->OBJ_ID == FIELD) continue;

		IsHit hit = Collision_Is_Hit_AABB(E_AABB, OBJ->Collision);

		if (hit.Is_Hit)
		{
			XMVECTOR vNormal = XMLoadFloat3(&hit.Normal);
			XMVECTOR vPush = vNormal * hit.Depth;

			Enemy_POS += vPush;

			XMStoreFloat3(&Position, Enemy_POS);
			E_AABB = GetAABB();
		}
	}
}

void Enemy::Enemy_Collision_Player(double dt)
{
	AABB P_AABB = Player_Get_AABB();
	AABB E_AABB = GetAABB();

	IsHit Hit = Collision_Is_Hit_AABB(P_AABB, E_AABB);

	if (Hit.Is_Hit)
	{
		// Push Out
		XMVECTOR Normal = XMLoadFloat3(&Hit.Normal);
		XMVECTOR Depth = XMLoadFloat(&Hit.Depth);
		XMVECTOR POS = XMLoadFloat3(&Position);

		POS += Normal * Depth;

		XMStoreFloat3(&Position, POS);
	}
}

void Enemy::Enemy_Collision_Enemy(double dt)
{
	std::vector<Enemy*>& activeList = Enemy_Manager::GetInstance().Get_Active_List();

	AABB My_AABB = GetAABB();
	XMVECTOR My_Pos = XMLoadFloat3(&Position);

	for (const auto& other : activeList)
	{
		// Skip Self, Or Dead Enemy
		if (!other->IsActive() || other->IsDead())
		{
			continue;
		}

		IsHit Hit = Collision_Is_Hit_AABB(My_AABB, other->GetAABB());

		if (Hit.Is_Hit)
		{
			XMVECTOR vNormal = XMLoadFloat3(&Hit.Normal);
			XMVECTOR vDepth = XMLoadFloat(&Hit.Depth);

			// Each Enemy Push Out Half Distance
			My_Pos += vNormal * vDepth * 0.5f;
		}
	}
	XMStoreFloat3(&Position, My_Pos);
}
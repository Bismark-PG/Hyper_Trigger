/*==============================================================================

	Manage Enemy Flight Type [Enemy_Flight.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef ENEMY_FLITGHT_H
#define ENEMY_FLITGHT_H
#include "Enemy.h"
#include <DirectXMath.h>
#include <Game_Model.h>
#include <Player.h>
using namespace DirectX;

constexpr float FLIGHT_HEIGHT_OFFSET = 2.5f;

class Enemy_Flight : public Enemy
{
public:
	Enemy_Flight(const XMFLOAT3& pos) : Enemy(pos)
	{
		m_MyType = EnemyType::FLIGHT;
		m_HP = 50;
		m_Speed = 3.0f;
		m_Scale = 1.0f;
		m_pModel = Model_Manager::GetInstance()->GetModel("Ball");

		if (m_pModel)
		{
			m_BottomOffset = fabs(m_pModel->Local_AABB.Min.y) * m_Scale;
		}
	}

	virtual void Update(double elapsed_time) override
	{
		float dt = static_cast<float>(elapsed_time);

		// AI State Machine
		XMFLOAT3 P_POS = Player_Get_POS();
		P_POS.y += FLIGHT_HEIGHT_OFFSET;

		// Get 3D Distance to Player
		float Chase_Range = static_cast<float>(pow(P_POS.x - Position.x, 2)
									+ pow(P_POS.y - Position.y, 2)
									+ pow(P_POS.z - Position.z, 2));
		float P_Distance = sqrt(Chase_Range);

		// Return to Spawn if too far
		if (P_Distance >= 100.0f)
		{
			Position = m_SpawnPos;
			m_State = EnemyState::IDLE;
		}
		else
		{
			switch (m_State)
			{
			case EnemyState::IDLE:
				// If Player is Within 20 -> CHASE
				if (P_Distance <= 20.0f) 
				{
					m_State = EnemyState::CHASE;
				}
				break;

			case EnemyState::CHASE:
				// If Player is Beyond 20 -> WAIT
				if (P_Distance > 20.0f) 
				{
					m_State = EnemyState::WAIT;
					m_WaitTimer = 3.0f;
				}
				// If Close Enough to Player -> Look at Player
				else if (P_Distance <= 5.0f)
				{
					Look_Player(P_POS);
				}
				// Chase Player
				else
				{
					Move_To_Target(P_POS, dt);
				}
				break;

			case EnemyState::WAIT:
				// If Player is Within 20, With Wait -> CHASE
				if (P_Distance <= 20.0f)
				{
					m_State = EnemyState::CHASE;
				}
				else
				{
					// Timer Decrease
					m_WaitTimer -= dt;
					if (m_WaitTimer <= 0.0f)
					{
						// Time's Up -> RETURN
						m_State = EnemyState::RETURN;
					}
				}
				break;

			case EnemyState::RETURN:
				// If Player is Within 20, With Return -> CHASE
				if (P_Distance <= 20.0f)
				{
					m_State = EnemyState::CHASE;
				}
				else
				{
					// Move to Spawn Point
					bool arrived = Move_To_Target(m_SpawnPos, dt);

					// Arrived at Spawn Point
					if (arrived)
					{
						m_State = EnemyState::IDLE;
					}
				}
				break;
			}
		}

		// Defense For Ground Penetration
		float groundY = Mash_Field_Get_Height(Position.x, Position.z);
		if (groundY > -1000.0f)
		{
			float feetPos = Position.y - m_BottomOffset;

			if (feetPos < groundY) 
			{
				Position.y = groundY + m_BottomOffset + FLIGHT_HEIGHT_OFFSET;
			}
		}

		// Collision Push Out
		// Enemy vs Enemy
		Enemy_Collision_Enemy(dt);
		// Enemy vs Player
		Enemy_Collision_Player(dt);
		// Enemy vs Map
		Enemy_Collision_Map(dt);
	}

private:
	void Look_Player(const XMFLOAT3& target)
	{
		XMVECTOR Target = XMLoadFloat3(&target);
		XMVECTOR Pos = XMLoadFloat3(&Position);
		XMVECTOR Dir = Target - Pos;
		Dir = XMVector3Normalize(Dir);

		XMFLOAT3 dirFloat; XMStoreFloat3(&dirFloat, Dir);
		Rotation.y = atan2f(dirFloat.x, dirFloat.z) + XM_PI;
	}

	bool Move_To_Target(const XMFLOAT3& target, float dt)
	{
		XMVECTOR Target = XMLoadFloat3(&target);
		XMVECTOR Pos = XMLoadFloat3(&Position);
		XMVECTOR Dir = Target - Pos;

		float remainDist = XMVectorGetX(XMVector3Length(Dir));

		if (remainDist < 0.1f)
			return true;

		Dir = XMVector3Normalize(Dir);
		Pos += Dir * m_Speed * dt;

		XMFLOAT3 dirFloat; XMStoreFloat3(&dirFloat, Dir);
		Rotation.y = atan2f(dirFloat.x, dirFloat.z) + XM_PI;

		XMStoreFloat3(&Position, Pos);
		return false;
	}
};

#endif // ENEMY_FILGHT_H
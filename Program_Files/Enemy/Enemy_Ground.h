/*==============================================================================

	Manage Enemy Ground Type [Enemy_Ground.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef ENEMY_GROUND_H
#define ENEMY_GROUND_H
#include "Enemy.h"
#include <DirectXMath.h>
#include <Game_Model.h>
#include <Player.h>
using namespace DirectX;
#include "Mash_Field.h"


class Enemy_Ground : public Enemy
{
public:
	Enemy_Ground(const XMFLOAT3& pos) : Enemy(pos)
	{
		m_MyType = EnemyType::GROUND;
		m_HP = 100;
		m_Speed = 2.0f;
		m_Scale = 2.0f;
		m_pModel = Model_Manager::GetInstance()->GetModel("Ball");

		if (m_pModel)
		{
			m_BottomOffset = fabs(m_pModel->Local_AABB.Min.y) * m_Scale;
		}
	}

	virtual void Update(double elapsed_time) override
	{
		float dt = static_cast<float>(elapsed_time);

		// Set Gravity
		m_Gravity += -9.8f * dt;
		Position.y += m_Gravity * dt;

		// Get Ground Height
		float groundY = Mash_Field_Get_Height(Position.x, Position.z);

		// If Landed on Ground
		if (groundY > -1000.0f)
		{
			// Adjust Position
			float feetPos = Position.y - m_BottomOffset;

			if (feetPos < groundY)
			{
				Position.y = groundY + m_BottomOffset;
				m_Gravity = 0.0f;
			}
		}

		// AI State Machine
		XMFLOAT3 P_POS = Player_Get_POS();

		// Get Distance to Player
		float Chase_Range = static_cast<float>(pow(P_POS.x - Position.x, 2)
									+ pow(P_POS.z - Position.z, 2));
		float P_Distance = sqrt(Chase_Range);

		// If Player is Far Away, Return to Spawn Point
		if (P_Distance >= 50.0f)
		{
			Position = m_SpawnPos;
			m_State = EnemyState::IDLE;
			m_Gravity = 0.0f;
		}
		else
		{
			switch (m_State)
			{
			case EnemyState::IDLE:
				// If Player is Within 10 -> CHASE
				if (P_Distance <= 10.0f)
				{
					m_State = EnemyState::CHASE;
				}
				break;

			case EnemyState::CHASE:
				// IF Player is Beyond 10 -> WAIT
				if (P_Distance > 10.0f)
				{
					m_State = EnemyState::WAIT;
					m_WaitTimer = 5.0f; // Set Wait Time
				}
				else
				{
					// Chase Player
					MoveToTarget(P_POS, dt);
				}
				break;

			case EnemyState::WAIT:
				// If Player is Within 10, With Wait -> CHASE
				if (P_Distance <= 10.0f)
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
				// If Player is Within 10, With Return -> CHASE
				if (P_Distance <= 10.0f)
				{
					m_State = EnemyState::CHASE;
				}
				else
				{
					// Move to Spawn Point
					bool arrived = MoveToTarget(m_SpawnPos, dt);

					// Arrived at Spawn Point
					if (arrived)
					{
						m_State = EnemyState::IDLE;
					}
				}
				break;
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
	bool MoveToTarget(const XMFLOAT3& target, float dt)
	{
		XMVECTOR Target = XMLoadFloat3(&target);
		XMVECTOR Pos = XMLoadFloat3(&Position);
		XMVECTOR Dir = Target - Pos;

		// Ignore Y Axis
		Dir = XMVectorSetY(Dir, 0.0f);

		float remainDist = XMVectorGetX(XMVector3Length(Dir));

		// Arrived
		if (remainDist < 0.1f)
		{
			return true;
		}

		Dir = XMVector3Normalize(Dir);
		Pos += Dir * m_Speed * dt;

		// Rotation
		XMFLOAT3 dirFloat;
		XMStoreFloat3(&dirFloat, Dir);
		Rotation.y = atan2f(dirFloat.x, dirFloat.z) + XM_PI;

		XMStoreFloat3(&Position, Pos);
		return false;
	}
};

#endif // ENEMY_GROUND_H
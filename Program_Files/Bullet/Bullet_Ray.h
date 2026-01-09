/*==============================================================================

    Manage Bullet Ray-Marching Type [Bullet_Ray.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef BULLET_RAY_H
#define BULLET_RAY_H

#include "Bullet.h"
#include "Enemy_Manager.h"
#include "Map_System.h"
using namespace DirectX;

class Bullet_Ray : public Bullet
{
public:
    Bullet_Ray(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir, BulletOwner owner)
        : Bullet(pos, dir, owner)
    {
		m_Damage = 20;       // Ray Bullet Damage
		m_MaxRange = 100.0f; // Range of the Ray
		m_StepSize = 0.5f;   // Step Size for Ray Marching
    }

    virtual void Update(double elapsed_time) override
    {
        XMVECTOR Ray_Pos = XMLoadFloat3(&m_Position);
        XMVECTOR Ray_Dir = XMLoadFloat3(&m_Direction);
        float Now_Dist = 0.0f;

        bool IsHitSomething = false;

        // --- Ray Marching Loop ---
        while (Now_Dist < m_MaxRange)
        {
			// Position Update
            Ray_Pos += Ray_Dir * m_StepSize;
            Now_Dist += m_StepSize;

            XMFLOAT3 CheckPos;
            XMStoreFloat3(&CheckPos, Ray_Pos);

            // Virtual Bullet AABB
            float Size = 0.1f;
            AABB RayAABB = {
                { CheckPos.x + Size, CheckPos.y + Size, CheckPos.z + Size }, // Max
                { CheckPos.x - Size, CheckPos.y - Size, CheckPos.z - Size }  // Min
            };

			// Map Collision Check
            if (Map_System_Check_Collision_AABB(RayAABB))
            {
                // Effect ?
                IsHitSomething = true;
                break;
            }

			// Enemy Collision Check
            if (m_Owner == BulletOwner::PLAYER)
            {
                Enemy* HitEnemy = Enemy_Manager::GetInstance().Check_Collision_AABB(RayAABB);
                if (HitEnemy != nullptr)
                {
					// Apply Damage
                    HitEnemy->OnDamage(m_Damage);
                    // Effect ?
                    IsHitSomething = true;
                    break;
                }
            }
        }

		// If hit something, Delete Bullet
        m_IsDead = true;
    }

    virtual void Draw() override
    {
		// Draw Bullet Effect or Debug Ray
    }

private:
    float m_MaxRange;
    float m_StepSize;
};

#endif // BULLET_RAY_H
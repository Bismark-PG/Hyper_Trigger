/*==============================================================================

    Manage Bullet Grenade Type [Bullet_Grenade.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef BULLET_GRENADE_H
#define BULLET_GRENADE_H

#include "Bullet.h"
#include "Game_Model.h"
#include "Map_System.h"

class Bullet_Grenade : public Bullet
{
public:
    Bullet_Grenade(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir, BulletOwner owner)
        : Bullet(pos, dir, owner)
    {
        using namespace DirectX;

		m_Damage = 50;     // Grenade Damage
		m_LifeTime = 3.0f; // Grenade Lifetime before explosion
		m_Gravity = -9.8f; // Grenade Gravity

		float Throw_Power = 15.0f; // Forward Throw Power
		float Up_Power = 5.0f;     // Upward Power

        XMVECTOR Dir = XMLoadFloat3(&m_Direction);
        XMVECTOR Vel = Dir * Throw_Power;

		// Projectile Upward Velocity
        Vel += XMVectorSet(0.0f, Up_Power, 0.0f, 0.0f);

        XMStoreFloat3(&m_Velocity, Vel);

		m_pModel = Model_Manager::GetInstance()->GetModel("Ball"); // Sample Model
    }

    virtual void Update(double elapsed_time) override
    {
        float dt = static_cast<float>(elapsed_time);
        using namespace DirectX;

        // Gravity
        m_Velocity.y += m_Gravity * dt;

		// Move
        XMVECTOR Pos = XMLoadFloat3(&m_Position);
        XMVECTOR Vel = XMLoadFloat3(&m_Velocity);
        Pos += Vel * dt;
        XMStoreFloat3(&m_Position, Pos);

		// AABB Collision Check
        AABB MyAABB = GetAABB();

        if (m_Position.y <= 0.5f)
        {
            m_Position.y = 0.5f;
			m_Velocity.y = -m_Velocity.y * 0.5f; // Bounce with Damping
			m_Velocity.x *= 0.8f; // Friction
            m_Velocity.z *= 0.8f;
        }

		// Lifetime Countdown
        m_LifeTime -= dt;
        if (m_LifeTime <= 0.0f)
        {
            // Boom!
            m_IsDead = true;
        }
    }

    virtual void Draw() override
    {
        if (m_pModel)
        {
            using namespace DirectX;
            XMMATRIX world = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
            world = XMMatrixScaling(0.5f, 0.5f, 0.5f) * world;
            ModelDraw(m_pModel, world);
        }
    }

private:
    DirectX::XMFLOAT3 m_Velocity = { 0,0,0 };
    float m_Gravity;
    MODEL* m_pModel = nullptr;

    AABB GetAABB()
    {
        return {
            { m_Position.x + 0.25f, m_Position.y + 0.25f, m_Position.z + 0.25f },
            { m_Position.x - 0.25f, m_Position.y - 0.25f, m_Position.z - 0.25f }
        };
    }
};

#endif // BULLET_GRENADE_H
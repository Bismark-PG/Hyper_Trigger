/*==============================================================================

    Manage Bullet And Bullet Type [Bullet.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef BULLET_H
#define BULLET_H

#include <DirectXMath.h>
#include "Collision.h"

//  Bullet Type
enum class BulletType
{
	RAY,     // Ray-Marching Bullet (Hit Scan)
	GRENADE  // Projectile Bullet (Grenade)
};

// Bullet Owner
enum class BulletOwner
{
    PLAYER,
    ENEMY
};

class Bullet
{
public:
    Bullet(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir, BulletOwner owner)
        : m_Position(pos), m_Direction(dir), m_Owner(owner)
    {
		// Direction Normalize
        DirectX::XMVECTOR vDir = DirectX::XMLoadFloat3(&m_Direction);
        vDir = DirectX::XMVector3Normalize(vDir);
        DirectX::XMStoreFloat3(&m_Direction, vDir);
    }
    virtual ~Bullet() {}

    virtual void Update(double elapsed_time) = 0;
    virtual void Draw() = 0;

    bool IsDead() const { return m_IsDead; }

protected:
    DirectX::XMFLOAT3 m_Position  = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_Direction = { 0.0f, 0.0f, 1.0f };  // Forward Direction
    BulletOwner m_Owner = BulletOwner::PLAYER;

    bool m_IsDead = false;
    int m_Damage = 10;
    float m_Speed = 0.0f;
    double m_LifeTime = 0.0f;
};
#endif // BULLET_H
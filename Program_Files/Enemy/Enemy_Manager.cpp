/*==============================================================================

	Manage Enemy Spawn System [Enemy_Manager.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Enemy_Manager.h"
#include "Enemy_Ground.h"
#include "Enemy_Flight.h"

// Set pool sizes1
static constexpr int POOL_SIZE_GROUND = 50;
static constexpr int POOL_SIZE_FLIGHT = 50;

void Enemy_Manager::Init()
{
    m_EnemyPool.clear();
    m_ActiveList.clear();

    // Ground Type
    for (int i = 0; i < POOL_SIZE_GROUND; ++i)
    {
        m_EnemyPool.push_back(new Enemy_Ground({ 0,0,0 }));
    }

	// Flight Type
    for (int i = 0; i < POOL_SIZE_FLIGHT; ++i)
    {
        m_ActiveList.push_back(new Enemy_Flight({ 0,0,0 }));
    }
}

void Enemy_Manager::Final()
{
    for (Enemy* e : m_EnemyPool)
    {
        delete e;
    }

    m_EnemyPool.clear();
    m_ActiveList.clear();
}

void Enemy_Manager::Update(double elapsed_time)
{
    // Update active list
    m_ActiveList.clear();

    for (Enemy* e : m_EnemyPool)
    {
        if (e->IsActive())
        {
            e->Update(elapsed_time);
            m_ActiveList.push_back(e);
        }
    }
}

void Enemy_Manager::Draw()
{
    for (Enemy* e : m_ActiveList)
    {
        e->Draw();
    }
}

void Enemy_Manager::Spawn(const DirectX::XMFLOAT3& pos, EnemyType type)
{
    // Search for an inactive enemy of the requested type
    for (Enemy* e : m_EnemyPool)
    {
        if (!e->IsActive() && e->GetType() == type)
        {
            // Spawn successful
            e->Activate(pos);
            return;
        }
    }
    // No available enemy found in the pool
}

Enemy* Enemy_Manager::Check_Collision_AABB(const AABB& box)
{
    for (Enemy* e : m_ActiveList)
    {
        if (Collision_Is_Hit_AABB(box, e->GetAABB()).Is_Hit)
        {
            return e;
        }
    }
    return nullptr;
}
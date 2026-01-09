/*==============================================================================

	Managed Billboard System [Billboard_Manager.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Billboard_Manager.h"
#include "Billboard_Object.h"
#include "Billboard_Target.h"
#include "Billboard_Effect.h"
#include "Texture_Manager.h"
#include <debug_ostream.h>
using namespace DirectX;

void Billboard_Manager::Init()
{
    m_ObjList.clear();
}

void Billboard_Manager::Final()
{
    for (auto* obj : m_ObjList)
    {
        delete obj;
    }

    m_ObjList.clear();
}

void Billboard_Manager::Update(double dt)
{
    for (auto* obj : m_ObjList)
    {
        if (obj->IsActive())
        {
            obj->Update(dt);
        }
    }
}

void Billboard_Manager::Draw()
{
    for (auto* obj : m_ObjList)
    {
        if (obj->IsActive())
        {
            obj->Draw();
        }
    }
}

void Billboard_Manager::Create(const XMFLOAT3& pos, Billboard_Type Type)
{
    int texID = -1;

    switch (Type)
    {
    case Billboard_Type::TARGET:
    {
        texID = Texture_Manager::GetInstance()->GetID("Target");

        if (texID == -1)
        {
            Debug::D_Out << "Texture Load Failed" << std::endl;
            return;
        }

        Billboard_Target* New_T = new Billboard_Target(texID, pos, 1.0f, 1.0f);
        New_T->Activate(pos);
        m_ObjList.push_back(New_T);
        break;
    }

    case Billboard_Type::OBJECT:
    {
        texID = Texture_Manager::GetInstance()->GetID("Logo");

        if (texID == -1)
        {
            Debug::D_Out << "Texture Load Failed" << std::endl;
            return;
        }

        Billboard_Object* New_O = new Billboard_Object(texID, pos, 5.0f, 5.0f);
        New_O->Activate(pos);
        m_ObjList.push_back(New_O);
        break;
    }
    }
}

void Billboard_Manager::Create_Effect(const XMFLOAT3& pos, int patternID, float scale, Effect_Type Type)
{
    // Need?
    switch (Type)
    {
    case Effect_Type::SMOKE:
        break;
    case Effect_Type::EXPLOSION:
        break;
    }

    Billboard_Effect* New_E = new Billboard_Effect(patternID, pos, scale);
    New_E->Activate(pos);
    m_ObjList.push_back(New_E);
}

Billboard_Target* Billboard_Manager::Check_Collision(const AABB& box)
{
    for (auto* obj : m_ObjList)
    {
        if (!obj->IsActive())
        {
            continue;
        }

        Billboard_Target* target = dynamic_cast<Billboard_Target*>(obj);

        if (target)
        {
            if (Collision_Is_Hit_AABB(box, target->GetAABB()).Is_Hit)
            {
                return target;
            }
        }
    }
    return nullptr;
}
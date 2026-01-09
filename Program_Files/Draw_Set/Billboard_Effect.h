/*==============================================================================

    Managed Billboard Target Type [Billboard_Target.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef BILLBOARD_EFFECT_H
#define BILLBOARD_EFFECT_H
#include "Billboard_Object.h"
#include "Sprite_Animation.h"

class Billboard_Effect : public Billboard_Object
{
public:
    Billboard_Effect(int patternID, const DirectX::XMFLOAT3& pos, float scale)
        : Billboard_Object(-1, pos, scale, scale)
    {
        m_PlayID = SpriteAni_CreatePlayer(patternID);
    }

    virtual ~Billboard_Effect()
    {
        if (m_PlayID != -1)
        {
            SpriteAni_DestroyPlayer(m_PlayID);
        }
    }

    virtual void Update(double dt) override
    {
		// If not active, skip
        if (m_PlayID != -1 && SpriteAni_IsStopped(m_PlayID))
        {
            Deactivate();
        }
    }

    virtual void Draw() override
    {
        if (!m_IsActive || m_PlayID == -1) return;

        Billboard_Draw_Animation(m_PlayID, m_Position, m_ScaleX, m_ScaleY);
    }

    void Reset(int patternID, const DirectX::XMFLOAT3& pos)
    {
        Activate(pos);
        if (m_PlayID != -1) SpriteAni_DestroyPlayer(m_PlayID);
        m_PlayID = SpriteAni_CreatePlayer(patternID);
    }

private:
    int m_PlayID = -1;
};

#endif // BILLBOARD_EFFECT_H
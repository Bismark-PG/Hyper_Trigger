/*==============================================================================

    Make Bullet [Bullet.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef BULLET_H
#define BULLET_H

#include <DirectXMath.h>
#include "Collision.h"

static constexpr int MAX_BULLET = 2048;

void Bullet_Initialize();
void Bullet_Finalize();

void Bullet_Update(double elapsed_time);
void Bullet_Draw();

void Bullet_Create(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity);
void Bullet_Destroy(int Index);

int Bullet_Get_Bullets_Count();
AABB Bullet_Get_AABB(int index);

#endif  //BULLET_H
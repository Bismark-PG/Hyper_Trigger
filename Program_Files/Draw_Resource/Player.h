/*==============================================================================

	Manage Player Logic [Player.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef PLAYER_H
#define PLAYER_H
#include <DirectXMath.h>
#include "Collision.h"

void Player_Initialize(const DirectX::XMFLOAT3& First_POS, const DirectX::XMFLOAT3& First_Front);
void Player_Finalize();

void Player_Update(double elapsed_time);
void Player_Draw();

void Player_Set_POS(DirectX::XMFLOAT3& POS);
const DirectX::XMFLOAT3& Player_Get_POS();
const DirectX::XMFLOAT3& Player_Get_Front();

AABB Player_Get_AABB();

#endif // PLAYER_H
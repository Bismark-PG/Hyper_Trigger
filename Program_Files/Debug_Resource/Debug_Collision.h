/*==============================================================================

	Manage Debug Collision [Debug_Collision.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef DEBUG_SETTINGS_H
#define DEBUG_SETTINGS_H
#include "Collision.h"
#include "direct3d.h"
#include <DirectXMath.h>

void Debug_Collision_Initialize(ID3D11Device* device);
void Debug_Collision_Finalize();

void Debug_Collision_Draw(const AABB& aabb, const DirectX::XMFLOAT4& color);

#endif // DEBUG_SETTINGS_H
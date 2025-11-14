/*==============================================================================

	Effect Draw [Effect.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef EFFECT_H
#define EFFECT_H

#include <DirectXMath.h>

void Effect_Initialize();
void Effect_Finalize();

void Effect_Update(double elapsed_time);
void Effect_Draw();

void Effect_Create(const DirectX::XMFLOAT2& position);

#endif // EFFECT_H
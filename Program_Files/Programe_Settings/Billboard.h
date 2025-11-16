/*==============================================================================

	Make Billboard [Billboard.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef BILLBOARD_H
#define BILLBOARD_H
#include <DirectXMath.h>

void Billboard_Initialize();
void Billboard_Finalize();

void Billboard_Draw(int Tex_ID, const DirectX::XMFLOAT3& POS,
	float Scale_X, float Scale_Y, const DirectX::XMFLOAT2& Pivot = { 0.0f,0.0f });


#endif // !BILLBOARD_H

/*==============================================================================

	Make Billboard [Billboard.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef BILLBOARD_H
#define BILLBOARD_H
#include <DirectXMath.h>

void Billboard_Initialize();
void Billboard_Finalize();

void Billboard_Draw(int Tex_ID, const DirectX::XMFLOAT3& pos, float Scale_X, float Scale_Y);


#endif // !BILLBOARD_H

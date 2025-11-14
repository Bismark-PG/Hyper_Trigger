/*==============================================================================

	Make Mash Field [Mash_Field.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef MASH_FIELD_H
#define MASH_FIELD_H
#include <d3d11.h>
#include <DirectXMath.h>

static constexpr int M_F_Vertex_Max = 10000;

void Mash_Field_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Mash_Field_Finalize(void);

void Mash_Field_Draw();

#endif // MASH_FIELD_H

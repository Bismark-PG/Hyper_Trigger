/*==============================================================================

	Draw Cube [Cube.h]
	
	Author : Choi HyungJoon

==============================================================================*/
#ifndef CUBE_H
#define CUBE_H

#include <d3d11.h>
#include <DirectXMath.h>
#include "Shader_Manager.h"

enum class Cube_Type
{
	BOX,
	SAMPLE
};

void Cube_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Cube_Finalize(void);

void Cube_Draw(DirectX::XMMATRIX W_Matrix, Shader_Filter Filter = Shader_Filter::ANISOTROPIC, Cube_Type Type = Cube_Type::BOX);
void Debug_Cube_Draw(DirectX::XMMATRIX W_Matrix, Shader_Filter Filter = Shader_Filter::MAG_MIP_POINT);

ID3D11ShaderResourceView* Get_Cube_Type(Cube_Type Type);

#endif //CUBE_H
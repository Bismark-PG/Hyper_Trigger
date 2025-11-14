/*==============================================================================

	Draw Sprite [Sprite.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef SPRITE_H
#define SPRITE_H

#include <d3d11.h>
#include <DirectXMath.h>

void Sprite_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Sprite_Finalize(void);

void Sprite_Begin();

// Show All Texture (Can Change Size)
void Sprite_Draw(int Tex_ID, float dx, float dy, float dw, float dh,
				 float angle = 0.0f, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

// Cut UV, And Rotation
void Sprite_UV_Draw(int Tex_ID, float dx, float dy, float dw, float dh,
					    float px, float py, float pw, float ph, float angle = 0.0f,
					    const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

#endif // SPRITE_H
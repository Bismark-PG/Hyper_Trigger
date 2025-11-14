/*==============================================================================
	
	Make Mash Field [Mash_Field.cpp]
	
	Author : Choi HyungJoon

==============================================================================*/
#include "Mash_Field.h"
#include "direct3d.h"
using namespace DirectX;
#include "Texture_Manager.h"
#include "Palette.h"
#include "Shader_Manager.h"
using namespace PALETTE;

static constexpr float FIELD_SIZE = 1.0f; // Field Size

static constexpr int FIELD_H_COUNT = 25; // Horizone Count
static constexpr int FIELD_V_COUNT = 25; // Vertical Count
static constexpr int FIELD_H_VERTEX_COUNT = FIELD_H_COUNT + 1;
static constexpr int FIELD_V_VERTEX_COUNT = FIELD_V_COUNT + 1;

static constexpr int NUM_VERTEX = FIELD_H_VERTEX_COUNT * FIELD_V_VERTEX_COUNT;
static constexpr int NUM_INDEX = 3 * 2 * FIELD_H_COUNT * FIELD_V_COUNT;

static ID3D11Buffer* Vertex_Buffer = nullptr; // 頂点バッファ
static ID3D11Buffer* Index_Buffer = nullptr; // 頂点バッファ

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

struct Vertex3D
{
	XMFLOAT3 POS;
	XMFLOAT3 Normal;
	XMFLOAT4 Color;
	XMFLOAT2 TexCord;
};

static Vertex3D Mash_Field_Vertex[NUM_VERTEX];

static unsigned short Mash_Field_Index[NUM_INDEX]
{

};

void Mash_Field_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex3D) * NUM_VERTEX; // sizeof(Mash_Field_Vertex);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	// Make Vertex Index
	for (int Z = 0; Z < FIELD_V_VERTEX_COUNT; Z++)
	{
		for (int X = 0; X < FIELD_H_VERTEX_COUNT; X++)
		{
			int Index = X + FIELD_H_VERTEX_COUNT * Z;
			float Blend_Ratio = static_cast<float>(X) / (FIELD_H_VERTEX_COUNT - 1);

			Mash_Field_Vertex[Index].POS = { X * FIELD_SIZE, A_Zero, Z * FIELD_SIZE };
			Mash_Field_Vertex[Index].Normal = { 0.0, 1.0f, 0.0f };
			Mash_Field_Vertex[Index].Color = { Blend_Ratio, A_Origin - Blend_Ratio, A_Zero, A_Origin };
			Mash_Field_Vertex[Index].TexCord = { static_cast<float>(X), static_cast<float>(Z) };
		}
	}

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = Mash_Field_Vertex;

	g_pDevice->CreateBuffer(&bd, &sd, &Vertex_Buffer);

	// Make Index Buffer
	int Index = 0;
	for (int V = 0; V < FIELD_V_COUNT; V++)
	{
		for (int H = 0; H < FIELD_H_COUNT; H++)
		{
			unsigned short Top_Left = H + V * FIELD_H_VERTEX_COUNT;
			unsigned short Top_Right = Top_Left + 1;
			unsigned short Bottom_Left = H + (V + 1) * FIELD_H_VERTEX_COUNT;
			unsigned short Bottom_Right = Bottom_Left + 1;

			//------------- Draw Type : \ -------------//
			// Draw First Triangle  (T.L -> B.L -> T.R)
			Mash_Field_Index[Index++] = Top_Left;
			Mash_Field_Index[Index++] = Bottom_Left;
			Mash_Field_Index[Index++] = Top_Right;

			// Draw Second Triangle (T.R -> B.R -> B.L)
			Mash_Field_Index[Index++] = Top_Right;
			Mash_Field_Index[Index++] = Bottom_Left;
			Mash_Field_Index[Index++] = Bottom_Right;

			//------------- Draw Type : / -------------//
			/*
			// Draw First Triangle  (T.L -> B.R -> T.R)
			Mash_Field_Index[Index++] = Top_Left;
			Mash_Field_Index[Index++] = Bottom_Right;
			Mash_Field_Index[Index++] = Top_Right;

			// Draw Second Triangle (T.L -> B.L -> B.R)
			Mash_Field_Index[Index++] = Top_Left;
			Mash_Field_Index[Index++] = Bottom_Left;
			Mash_Field_Index[Index++] = Bottom_Right;
			*/

			// Mash_Field_Index[Index + 0] = H + (V + 0) * FIELD_H_VERTEX_COUNT;
			// Mash_Field_Index[Index + 1] = H + (V + 1) * FIELD_H_VERTEX_COUNT + 1;
			// Mash_Field_Index[Index + 2] = Mash_Field_Index[Index + 0] + 1;
			// Mash_Field_Index[Index + 3] = Mash_Field_Index[Index + 0];
			// Mash_Field_Index[Index + 4] = Mash_Field_Index[Index + 1] - 1;
			// Mash_Field_Index[Index + 5] = Mash_Field_Index[Index + 1];
			// Index += 6;
		}
	}

	// Creat Index Buffer
	bd.ByteWidth = sizeof(unsigned short) * NUM_INDEX; // if Arr >> "sizeof(Cube_Index)" is OK
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	sd.pSysMem = Mash_Field_Index;

	g_pDevice->CreateBuffer(&bd, &sd, &Index_Buffer);
}

void Mash_Field_Finalize()
{
	SAFE_RELEASE(Vertex_Buffer);
	SAFE_RELEASE(Index_Buffer);
}

void Mash_Field_Draw()
{
	// シェーダーを描画パイプラインに設定
	Shader_M->Begin3D_For_Field();

	// Texture_SetTexture(Cube_Tex_ID);
	ID3D11ShaderResourceView* Grass = Texture_M->Get_Shader_Resource_View(Texture_M->GetID("Mash_Grass"));
	ID3D11ShaderResourceView* Rock = Texture_M->Get_Shader_Resource_View(Texture_M->GetID("Mash_Dirt"));
	Shader_M->SetFieldTextures(Grass, Rock);


	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &Vertex_Buffer, &stride, &offset);

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set Index Buffer
	// unsigned short = DXGI_FORMAT_R16_UINT
	// unsigned int	  = DXGI_FORMAT_R32_UINT
	g_pContext->IASetIndexBuffer(Index_Buffer, DXGI_FORMAT_R16_UINT, 0);

	// 頂点シェーダーに座標変換行列を設定
	float Offset_X = -FIELD_H_VERTEX_COUNT * FIELD_SIZE * 0.5f;
	float Offset_Z = -FIELD_V_VERTEX_COUNT * FIELD_SIZE * 0.5f;

	Shader_M->SetWorldMatrix3D(XMMatrixTranslation(Offset_X, 0.0f, Offset_Z));

	// Just Draw
	// g_pContext->Draw(NUM_VERTEX, 0); 

	// Draw With Index
	g_pContext->DrawIndexed(NUM_INDEX, 0, 0);
}

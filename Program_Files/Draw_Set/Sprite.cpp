/*==============================================================================

	Draw Sprite [Sprite.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "direct3d.h"
#include "Shader_Manager.h"
#include "Debug_ostream.h"
#include "Sprite.h"
#include "Texture_Manager.h"

static constexpr int NUM_VERTEX = 4; // 頂点数

static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;


// 頂点構造体
struct Vertex
{
	XMFLOAT3 position; // 頂点座標
	XMFLOAT4 color;
	XMFLOAT2 UV;
};

void Sprite_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// デバイスとデバイスコンテキストのチェック
	if (!pDevice || !pContext) {
		Debug::D_Out << "Sprite_Initialize() : 与えられたデバイスかコンテキストが不正です" << std::endl;
		return;
	}

	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);
}

void Sprite_Finalize(void)
{
	SAFE_RELEASE(g_pVertexBuffer);
}

void Sprite_Begin()
{	
	// 頂点情報を書き込み
	// Make Camera (Zoom In or Out, More etc...)
	const float SCREEN_WIDTH  = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

	// 頂点シェーダーに変換行列を設定
	Shader_M->SetProjectionMatrix2D(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));
}

// Show All Texture (Can Change Size)
void Sprite_Draw(int Tex_ID, float dx, float dy, float dw, float dh, float angle, const DirectX::XMFLOAT4& color)
{
	if (Tex_ID < 0) return;

	float Tex_Width  = static_cast<float>(Texture_M->Get_Width(Tex_ID));
	float Tex_Height = static_cast<float>(Texture_M->Get_Height(Tex_ID));

	Sprite_UV_Draw(Tex_ID, dx, dy, dw, dh, 0.0f, 0.0f, Tex_Width, Tex_Height, angle, color);
}

// Rotation
void Sprite_UV_Draw(int Tex_ID, float dx, float dy, float dw, float dh,
	float px, float py, float pw, float ph, float angle, const DirectX::XMFLOAT4& color)
{
	// シェーダーを描画パイプラインに設定
	Shader_M->Begin2D(Shader_Filter::MAG_MIP_LINEAR);

	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;

	// 画面の左上から右下に向かう線分を描画する
	v[0].position = { -0.5f,	-0.5f,	0.0f }; // 左上
	v[1].position = { +0.5f,	-0.5f,	0.0f }; // 右上
	v[2].position = { -0.5f,	+0.5f,	0.0f }; // 左下
	v[3].position = { +0.5f,	+0.5f,	0.0f }; // 右下

	v[0].color = color;
	v[1].color = color;
	v[2].color = color;
	v[3].color = color;

	// Get Texture Size
	float tw = static_cast<float>(Texture_M->Get_Width(Tex_ID));
	float th = static_cast<float>(Texture_M->Get_Height(Tex_ID));

	// Set UV Size
	float U0 = px / tw;
	float U1 = (px + pw) / tw;
	float V0 = py / th;
	float V1 = (py + ph) / th;

	// Cut Texture With Using UV Tool
	//	U = Width	||   V = Height
	v[0].UV = { U0, V0 };
	v[1].UV = { U1, V0 };
	v[2].UV = { U0, V1 };
	v[3].UV = { U1, V1 };

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	// Rotation Shader Setting
	XMMATRIX Scale = XMMatrixScaling(dw, dh, 1.0f); // Scale
	XMMATRIX Rotation = XMMatrixRotationZ(XMConvertToRadians(angle)); // Rotation
	XMMATRIX Translation = XMMatrixTranslation(dx + (dw / 2), dy + (dh / 2), 0.0f); // 平行移動

	Shader_M->SetWorldMatrix2D(Scale * Rotation * Translation);

	// Auto Reset
	// If You Want Rotate Or Move Many Texture Use This In Main
	// Shader_SetWorldMatrix(XMMatrixIdentity());

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Setting Texture
	ID3D11ShaderResourceView* srv = Texture_M->Get_Shader_Resource_View(Tex_ID);
	Shader_M->SetTexture2D(srv);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}
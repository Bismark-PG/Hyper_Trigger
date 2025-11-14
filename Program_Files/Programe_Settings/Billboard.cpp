/*==============================================================================

	Make Billboard [Billboard.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Billboard.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "Texture_Manager.h"
#include "Game_Texture.h"
#include "Shader_Manager.h"

static constexpr int NUM_VERTEX = 4;

static ID3D11Buffer* Vertex_Buffer = nullptr;

struct Vertex_Billboard
{
	XMFLOAT3 position;
	XMFLOAT4 color;   
	XMFLOAT2 texcord; 
};

void Billboard_Initialize()
{
	static Vertex_Billboard Billboard[]
	{
		{{-0.5f, 0.5f,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.0f, 0.0f}},
		{{ 0.5f,-0.5f,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {1.0f, 0.0f}},
		{{-0.5f,-0.5f,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.0f, 1.0f}},
		{{ 0.5f, 0.5f,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {1.0f, 1.0f}}
	};

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex_Billboard) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = Billboard;

	Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &Vertex_Buffer);
}

void Billboard_Finalize()
{
	SAFE_RELEASE(Vertex_Buffer);
}

void Billboard_Draw(int Tex_ID, const DirectX::XMFLOAT3& pos, float Scale_X, float Scale_Y)
{
	// シェーダーを描画パイプラインに設定
	Shader_M->Begin3D();

	// Color Reset
	Shader_M->SetDiffuseColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	ID3D11ShaderResourceView* SRV = Texture_M->Get_Shader_Resource_View(Tex_ID);
	Shader_M->SetTexture3D(SRV);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex_Billboard);
	UINT offset = 0;
	Direct3D_GetContext()->IASetVertexBuffers(0, 1, &Vertex_Buffer, &stride, &offset);

	// プリミティブトポロジ設定
	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点シェーダーに座標変換行列を設定
	XMMATRIX Scale = XMMatrixScaling(Scale_X, Scale_Y, 1.0f);
	XMMATRIX mtx = XMMatrixTranslation(pos.x, pos.y, pos.z);

	Shader_M->SetWorldMatrix3D(Scale * mtx);

	// Just Draw
	Direct3D_GetContext()->Draw(NUM_VERTEX, 0);
}

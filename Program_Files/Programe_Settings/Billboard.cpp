/*==============================================================================

	Make Billboard [Billboard.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Billboard.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "Texture_Manager.h"
#include "Shader_Manager.h"
#include <Player_Camera.h>
#include <wrl/client.h>

static constexpr int NUM_VERTEX = 4;

static Microsoft::WRL::ComPtr<ID3D11Buffer> Vertex_Buffer = nullptr;

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
		{{-0.5f, 0.5f,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.0f, 0.0f}}, // 0 : LU
		{{ 0.5f, 0.5f,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {1.0f, 0.0f}}, // 1 : RU
		{{-0.5f,-0.5f,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.0f, 1.0f}}, // 2 : LD
		{{ 0.5f,-0.5f,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {1.0f, 1.0f}}  // 3 : RD
	};

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex_Billboard) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = Billboard;

	Direct3D_GetDevice()->CreateBuffer(&bd, &sd, Vertex_Buffer.GetAddressOf());
}	

void Billboard_Finalize()
{
	Vertex_Buffer.Reset();
}

void Billboard_Draw(int Tex_ID, const DirectX::XMFLOAT3& POS,
	float Scale_X, float Scale_Y, const DirectX::XMFLOAT2& Pivot)
{
	// Set UV
	UV_Parameter UV = {};
	UV.scale = { 0.2f, 0.5f };
	UV.translation = { 0.2f * 3, 0.5f };
	Shader_M->SetUVParameter(UV);
	Shader_M->Begin_Billboard();

	// Set Color
	Shader_M->SetDiffuseColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	// Set Texture
	ID3D11ShaderResourceView* SRV = Texture_M->Get_Shader_Resource_View(Tex_ID);
	if (SRV)
		Direct3D_GetContext()->PSSetShaderResources(0, 1, &SRV);

	// Set Vertex Buffer
	UINT stride = sizeof(Vertex_Billboard);
	UINT offset = 0;
	Direct3D_GetContext()->IASetVertexBuffers(0, 1, Vertex_Buffer.GetAddressOf(), &stride, &offset);

	// Set Topology
	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Set Offset
	XMMATRIX Pivot_Offset = XMMatrixTranslation(-Pivot.x, -Pivot.y, 0.0f);

	// Get Camera Rotation
	XMFLOAT4X4 mtxCamera = PlayerCamera_GetViewMatrix();
	mtxCamera._41 = mtxCamera._42 = mtxCamera._43 = 0.0f;

	// Get Inverse_Matrix
	XMMATRIX Inverse_mtx = XMMatrixTranspose(XMLoadFloat4x4(&mtxCamera));

	// Get Scale And Translation
	XMMATRIX Scale = XMMatrixScaling(Scale_X, Scale_Y, 1.0f);
	XMMATRIX Trans = XMMatrixTranslation(POS.x + Pivot.x, POS.y + Pivot.y, POS.z);

	// Get Final mtx
	XMMATRIX mtxWorld = Pivot_Offset * Scale * Inverse_mtx * Trans;

	// Set World mtx
	Shader_M->SetWorldMatrix3D(mtxWorld);

	// Draw
	Direct3D_GetContext()->Draw(NUM_VERTEX, 0);
}

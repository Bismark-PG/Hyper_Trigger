/*==============================================================================
	
	Make Grid [Grid.cpp]
	
	Author : Choi HyungJoon

==============================================================================*/
#include "Grid.h"
#include "direct3d.h"
#include <DirectXMath.h>
#include "Shader_Manager.h"
using namespace DirectX;

static constexpr int GRID_H_COUNT = 10;
static constexpr int GRID_V_COUNT = 10;
static constexpr int GRID_H_LINE_COUNT = GRID_H_COUNT + 1;
static constexpr int GRID_V_LINE_COUNT = GRID_V_COUNT + 1;
static constexpr int NUM_VERTEX = GRID_H_LINE_COUNT * 2 + GRID_V_LINE_COUNT * 2;

static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ


// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

//3D頂点構造体
struct Vertex3D
{
	XMFLOAT3 position; // 頂点座標
	XMFLOAT4 color;    // 色
};

static Vertex3D g_GridVertex[NUM_VERTEX]{};


void Grid_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;

	g_pContext = pContext;

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex3D) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = g_GridVertex;

	float x = -5.0f;
	for (int i = 0; i < GRID_H_LINE_COUNT * 2;i += 2) {
		g_GridVertex[i  ] = { { x, 0.0f, 5.0f },{0.0f,1.0f,0.0f,1.0f} };
		g_GridVertex[i+1] = { { x, 0.0f,-5.0f },{0.0f,1.0f,0.0f,1.0f} };
		x +=1.0f;
	}

	float z = -5.0f;
	for (int i = GRID_V_LINE_COUNT * 2; i < NUM_VERTEX;i += 2) {
		g_GridVertex[i    ] = { { 5.0f, 0.0f, z },{0.0f,1.0f,0.0f,1.0f} };
		g_GridVertex[i + 1] = { {-5.0f, 0.0f, z },{0.0f,1.0f,0.0f,1.0f} };
		z += 1.0f;
	}

	g_pDevice->CreateBuffer(&bd, &sd, &g_pVertexBuffer);

}

void Grid_Finalize(void)
{
	SAFE_RELEASE(g_pVertexBuffer);
}

void Grid_Draw(void)
{
	// シェーダーを描画パイプラインに設定
	Shader_Manager::GetInstance()->Begin3D(Shader_Filter::MAG_MIP_POINT);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	//ワールド座標変換行列の作成
	XMMATRIX mtxWorld = XMMatrixIdentity(); //単位行列の作成

	// 頂点シェーダーに座標変換行列を設定
	Shader_Manager::GetInstance()->SetWorldMatrix3D(mtxWorld);

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}
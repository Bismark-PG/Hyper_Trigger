/*==============================================================================

	Manage Debug Collision [Debug_Collision.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Debug_Collision.h"
#include "Texture_Manager.h"
#include "Cube.h"

using namespace DirectX;

static ID3D11RasterizerState* Wire_Frame_State = nullptr;
static int Debug_Texture_ID = -1;

void Debug_Collision_Initialize(ID3D11Device* device)
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_NONE;     
	desc.DepthClipEnable = true;
	desc.MultisampleEnable = true;

	device->CreateRasterizerState(&desc, &Wire_Frame_State);

	Debug_Texture_ID = Texture_Manager::GetInstance()->GetID("TextSample");
}

void Debug_Collision_Finalize()
{
	SAFE_RELEASE(Wire_Frame_State);
}

void Debug_Collision_Draw(const AABB& Collision, const DirectX::XMFLOAT4& color)
{
	ID3D11DeviceContext* context = Direct3D_GetContext();
	if (!context || !Wire_Frame_State) return;

	// Set AABB Scale 
	XMVECTOR vMin = XMLoadFloat3(&Collision.Min);
	XMVECTOR vMax = XMLoadFloat3(&Collision.Max);
	XMVECTOR vSize = vMax - vMin;

	// Get Middle Of AABB
	XMVECTOR vCenter = (vMin + vMax) * 0.5f;

	// Scaling Cude Scale
	XMMATRIX mtxScale = XMMatrixScalingFromVector(vSize);
	XMMATRIX mtxTrans = XMMatrixTranslationFromVector(vCenter);
	XMMATRIX world = mtxScale * mtxTrans;

	// Save Rasterizer State
	ID3D11RasterizerState* oldState = nullptr;
	context->RSGetState(&oldState);

	// Change Rasterizer To Wire Frame
	context->RSSetState(Wire_Frame_State);

	// Set Shader Color
	Shader_Manager::GetInstance()->SetDiffuseColor(color);
	Shader_Manager::GetInstance()->SetTexture3D(Texture_Manager::GetInstance()->
		Get_Shader_Resource_View(Debug_Texture_ID));

	// Begin Shader
	Shader_Manager::GetInstance()->Begin3D(Shader_Filter::MAG_MIP_POINT); 
	Shader_Manager::GetInstance()->SetDiffuseColor(color);
	Shader_Manager::GetInstance()->SetTexture3D(Texture_Manager::GetInstance()->
		Get_Shader_Resource_View(Debug_Texture_ID));

	Debug_Cube_Draw(world);

	// Rollback Rasterizer State
	context->RSSetState(oldState);
	SAFE_RELEASE(oldState);
}
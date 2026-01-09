/*==============================================================================

	Game [Game.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Game.h"
#include "Effect.h"
#include "Cube.h"
#include "Grid.h"
#include "direct3d.h"
#include "KeyLogger.h"
#include <DirectXMath.h>
#include "Mash_Field.h"
#include "Player.h"
#include "Player_Camera.h"
#include "Palette.h"
#include "Model.h"
#include "Game_Model.h"
#include "Game_UI.h"
#include "Billboard.h"
#include "Debug_Camera.h"
#include "Map_System.h"
#include "Sky.h"
#include "Enemy.h"
#include "Enemy_Manager.h"
#include "Light_Manager.h"
#include "Texture_Manager.h"
#include "Billboard_Manager.h"
#include "Bullet_Manager.h"
using namespace DirectX;
using namespace PALETTE;

static XMFLOAT3 Player_First_POS = { 0.0f, 5.0f, -5.0f };

static bool Is_Debug_Mode = false;
static bool Is_Sights_Change = false;

void Game_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	Player_Initialize(Player_First_POS, { 0.0f, 0.0f, 1.0f });
	Player_Camera_Initialize();

	Grid_Initialize(pDevice, pContext);
	Cube_Initialize(pDevice, pContext);
	Mash_Field_Initialize(pDevice, pContext);
	Map_System_Initialize();

	Sky_Initialize();
	Enemy_Manager::GetInstance().Init();

	Game_UI_Initialize();
}

void Game_Finalize()
{
	Game_UI_Finalize();
	
	Enemy_Manager::GetInstance().Final();
	Sky_Finalize();

	Map_System_Finalize();
	Mash_Field_Finalize();
	Cube_Finalize();
	Grid_Finalize();

	Player_Camera_Finalize();
	Player_Finalize();
}

void Game_Update(double elapsed_time)
{
	if (KeyLogger_IsTrigger(KK_V) && !Is_Sights_Change)
	{
		Is_Sights_Change = true;
		Player_Camera_Change_Sights();
		Is_Sights_Change = false;
	}

	if (KeyLogger_IsTrigger(KK_F1))
	{
		Is_Debug_Mode = !Is_Debug_Mode;

		Debug_Mode_Switcher();
		Debug_Mode_Set();

		Debug_Camera_Set_Position(Player_Camera_Get_Current_POS());
		Debug_Camera_Set_Rotation(Player_Camera_Get_Yaw(), Player_Camera_Get_Pitch());
	}

	if (Is_Debug_Mode)
	{
		Debug_Camera_Update(elapsed_time);
	}
	else
	{
		Light_Manager::GetInstance().Global_Light_Update(elapsed_time);

		Map_System_Update(elapsed_time);
		Player_Update(elapsed_time);
		Bullet_Manager::Instance().Update(elapsed_time);
		Player_Camera_Update(elapsed_time);

		Sky_Update();
		Enemy_Manager::GetInstance().Update(elapsed_time);
	}
}

void Game_Draw()
{
	Light_Manager::GetInstance().Global_Light_Set_Up();

	Direct3D_SetDepthEnable(true);
	Shader_Manager::GetInstance()->Begin3D();

	Sky_Draw();
	Map_System_Draw();
	Player_Draw();

	Bullet_Manager::Instance().Draw();
	Enemy_Manager::GetInstance().Draw();

	Shader_Manager::GetInstance()->SetAlphaBlend(true);
	Billboard_Manager::Instance().Draw();

	// Draw UI
	if (!Is_Debug_Mode)
	{
		Direct3D_SetDepthEnable(false);
		Shader_Manager::GetInstance()->Begin2D();
		Game_UI_Draw();
	}
}

bool Get_Debug_Mode_State()
{
	return Is_Debug_Mode;
}

void Set_Debug_Mode_State(bool State)
{
	Is_Debug_Mode = State;
}

// Make pyramid
//int d = 1, i = 5;
//for (int j = 0; j < 2; j++)
//{
//	for (int k = 0; k < 2; k++)
//	{
//		XMMATRIX mtxOffsec = XMMatrixTranslation(-2.5f + i * j, 0.0f, 2.5f - i * k);
//		
//		for (int y = 0; y < 5 * d; y++)
//		{
//			for (int z = 0; z < 5 * d - y; z++)
//			{
//				for (int x = 0; x < 5 * d - y; x++)
//				{
//					XMMATRIX mtxTrans = XMMatrixTranslation(-2.0f * d + x + 0.5f * y, 0.5f + y, -2.0f * d + z + 0.5f * y);
//					XMMATRIX mtxRotateY = XMMatrixRotationY(XMConvertToRadians(g_Angle));
//					XMMATRIX mtxScale = XMMatrixScaling(1.0f, g_Scale, 1.0f);
//					
//					XMMATRIX mtxWorld = mtxTrans * mtxRotateY * mtxOffsec * mtxScale;
//					
//					Cube_Draw(mtxWorld);
//				}
//			}
//		}
//	}
//}
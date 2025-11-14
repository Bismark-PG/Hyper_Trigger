/*==============================================================================

	Manage Player Logic [Player.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Player.h"
#include "KeyLogger.h"
#include "model.h"
#include "Shader_Manager.h"
#include "Player_Camera.h" 
#include "Game_Model.h"
#include "Debug_Collision.h"
#include "Palette.h"
using namespace DirectX;
using namespace PALETTE;

constexpr float Player_drag = 5.0f;

static XMFLOAT3 Player_Pos	 = {};
static XMFLOAT3 Player_Front = { 0.0f, 0.0f, 1.0f };
static XMFLOAT3 Player_Vel	 = {};
static XMVECTOR Player_Speed = {};
static float Player_Height_Offset = 0.0f;

static XMVECTOR Gravity		= { 0.0f, -7.5f, 0.0f };
static XMVECTOR Walk_Speed	= { 20.f, 0.0, 20.f };
static XMVECTOR Run_Speed	= { 50.f, 0.0, 50.f };

static MODEL* Player_Model{nullptr};
static bool Is_Jump = false;
static bool Is_Run = false;
static bool Is_Run_Toggle = false;

// If Input Model, Can Change Chara
void Player_Initialize(const XMFLOAT3& First_POS, const XMFLOAT3& First_Front)
{
	Player_Pos = First_POS;
	Player_Vel = { 0.0f, 0.0f, 0.0f };
	XMStoreFloat3(&Player_Front, XMVector3Normalize(XMLoadFloat3(&First_Front)));

	Player_Model = Model_M->GetModel("player");

	Is_Jump = false;
	Is_Run = false;
	Is_Run_Toggle = false;
}

void Player_Finalize()
{
	ModelRelease(Player_Model);
}

void Player_Update(double elapsed_time)
{
	if (KeyLogger_IsTrigger(KK_F4)) // toggle mode << debug
		Is_Run_Toggle = !Is_Run_Toggle;

	if (Is_Run_Toggle)
	{
		if (KeyLogger_IsTrigger(KK_LEFTSHIFT))
			Is_Run = !Is_Run;
	}
	else
	{
		if ((KeyLogger_IsPressed(KK_LEFTSHIFT) && !Is_Run) || (KeyLogger_IsReleased(KK_LEFTSHIFT) && Is_Run))
			Is_Run = !Is_Run;
	}


	if (Is_Run)
		Player_Speed = Run_Speed;
	else
		Player_Speed = Walk_Speed;


	float dt = static_cast<float>(elapsed_time);
	XMVECTOR POS = XMLoadFloat3(&Player_Pos);
	XMVECTOR Vel = XMLoadFloat3(&Player_Vel);

	if (XMVectorGetY(POS) <= 0.0f)
	{
		POS = XMVectorSetY(POS, 0.0f);
		Vel = XMVectorSetY(Vel, 0.0f);
		Is_Jump = false;
	}

	if (KeyLogger_IsTrigger(KK_SPACE) && !Is_Jump)
	{
		Is_Jump = true;
		Vel += XMVECTOR{ 0.0f, 5.0f, 0.0f };
	}

	Vel += Gravity * dt;

	XMVECTOR Dir{};
	XMVECTOR Front = XMLoadFloat3(&Player_Camera_Get_Front());

	XMVECTOR Flat_Front = XMVector3Normalize(XMVectorSetY(Front, 0.0f));
	XMVECTOR Flat_Right = XMVector3Cross({ 0.0f,1.0f,0.0f }, Flat_Front);

	if (KeyLogger_IsPressed(KK_W))
	{
		Dir += Flat_Front;
	}
	if (KeyLogger_IsPressed(KK_S))
	{
		Dir -= Flat_Front;
	}
	if (KeyLogger_IsPressed(KK_D))
	{
		Dir += Flat_Right;
	}
	if (KeyLogger_IsPressed(KK_A))
	{
		Dir -= Flat_Right;
	}

	Dir = XMVector3Normalize(Dir);

	Vel += Dir * Player_Speed * dt;
	
	XMVECTOR Horizontal_Vel = XMVectorMultiply(Vel, XMVectorSet(1.0f, 0.0f, 1.0f, 0.0f));
	Horizontal_Vel -= Horizontal_Vel * Player_drag * dt;
	Vel = XMVectorMultiply(Vel, XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f)) + Horizontal_Vel;

	POS += Vel * dt;

	XMStoreFloat3(&Player_Front, Flat_Front);

	XMStoreFloat3(&Player_Pos, POS);
	XMStoreFloat3(&Player_Vel, Vel);
}

void Player_Draw()
{
	if (!Player_Model) return;

	Shader_M->SetLightSpecular(Player_Camera_Get_POS(), 164.0f, { 1.0f, 1.0f, 1.0f, 1.0f });

	// Get Movement
	XMMATRIX t = XMMatrixTranslation(Player_Pos.x, Player_Pos.y + Player_Height_Offset, Player_Pos.z);
	
	// Get Yaw
	float yaw = atan2f(Player_Front.x, Player_Front.z);

	// Reverse Player
	float modelYaw = yaw + XM_PI;
	XMMATRIX r = XMMatrixRotationY(modelYaw);
	XMMATRIX world = r * t;

	ModelDraw(Player_Model, world);
	Debug_Collision_Draw(Player_Get_AABB(), Red);
}

void Player_Set_POS(XMFLOAT3& POS)
{
	Player_Pos = POS;
}

const XMFLOAT3& Player_Get_POS()
{
	return Player_Pos;
}

const XMFLOAT3& Player_Get_Front()
{
	return Player_Front;
}

AABB Player_Get_AABB()
{
	float w = 0.25f;
	float h = 1.75f;
	float d = 0.25f;

	return AABB
	{
		{ Player_Pos.x + w, Player_Pos.y + h, Player_Pos.z + d }, // Max
		{ Player_Pos.x - w, Player_Pos.y	, Player_Pos.z - d }  // Min
	};
}

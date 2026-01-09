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
#include "Map_System.h"
#include <Game.h>
#include <Mash_Field.h>
using namespace DirectX;
using namespace PALETTE;

constexpr float Player_drag = 5.0f;

static XMFLOAT3 Player_Pos	 = {};
static XMFLOAT3 Player_Start_POS = {};
static XMFLOAT3 Player_Front = { 0.0f, 0.0f, 1.0f };
static XMFLOAT3 Player_Vel	 = {};
static XMVECTOR Player_Speed = {};
static float Player_Height_Offset = 0.0f;

static XMVECTOR Gravity		= { 0.0f, -15.0f, 0.0f };
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
	Player_Start_POS = First_POS; 
	Player_Vel = { 0.0f, 0.0f, 0.0f };
	XMStoreFloat3(&Player_Front, XMVector3Normalize(XMLoadFloat3(&First_Front)));

	Player_Model = Model_Manager::GetInstance()->GetModel("Player");

	Is_Jump = false;
	Is_Run = false;
	Is_Run_Toggle = false;
}

void Player_Finalize()
{
	Player_Model = nullptr;
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


	// Jump Logic
	if (KeyLogger_IsTrigger(KK_SPACE) && !Is_Jump)
	{
		Is_Jump = true;
		Vel = XMVectorSetY(Vel, 0.0f);
		Vel += XMVECTOR{ 0.0f, 10.0f, 0.0f };
	}

	// Gravity Apply
	Vel += Gravity * dt;

	// Movement Logic
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
	
	// Friction
	XMVECTOR Horizontal_Vel = XMVectorMultiply(Vel, XMVectorSet(1.0f, 0.0f, 1.0f, 0.0f));
	Horizontal_Vel -= Horizontal_Vel * Player_drag * dt;
	Vel = XMVectorMultiply(Vel, XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f)) + Horizontal_Vel;

	// Apply Velocity
	POS += Vel * dt;
	XMStoreFloat3(&Player_Pos, POS);

	// Ground Collision Check
	float Ground_Y = Mash_Field_Get_Height(Player_Pos.x, Player_Pos.z);

	// Landed on Ground
if (Ground_Y > -1000.0f)
	{
	// Dive into Ground Check Conditions
	// 1. If Player is below Ground (Player_Pos.y < Ground_Y)
	// 2. If Player is Falling (Vel.y <= 0.0f)
	// 3. If Player is Close Enough to Ground (Dist_To_Ground < Landing)	

		float Dist_To_Ground = Ground_Y - Player_Pos.y;
		constexpr float Landing = 2.0f; // Landing Threshold

		if (Player_Pos.y < Ground_Y && XMVectorGetY(Vel) <= 0.0f && Dist_To_Ground < Landing)
		{
			Player_Pos.y = Ground_Y;
			Vel = XMVectorSetY(Vel, 0.0f);
			Player_Vel.y = 0.0f;
			Is_Jump = false;
		}
	}

	// Respawn Check
	if (Player_Pos.y < -100.0f)
	{
		Player_Pos = Player_Start_POS;
		Player_Vel = { 0.0f, 0.0f, 0.0f };
		Vel = XMLoadFloat3(&Player_Vel);
	}

	// Check Collision With Map Objects
	int OBJ_Count = Map_System_Get_Object_Count();
	float skinWidth = 0.05f;

	// Get Player AABB
	AABB Vertical_AABB	 = Player_Get_AABB();
	AABB Horizontal_AABB = Player_Get_AABB();

	// Get Flat Front
	Vertical_AABB.Min.x += skinWidth; Vertical_AABB.Max.x -= skinWidth;
	Vertical_AABB.Min.z += skinWidth; Vertical_AABB.Max.z -= skinWidth;

	for (int i = 0; i < OBJ_Count; i++)
	{
		const MapObject* OBJ = Map_System_Get_Object(i);
		if (OBJ->OBJ_ID == FIELD)
			continue;

		// Y Axis Collision
		IsHit Hit = Collision_Is_Hit_AABB(Vertical_AABB, OBJ->Collision);

		if (Hit.Is_Hit && Hit.Normal.y != 0.0f)
		{
			// Y Push Out
			XMVECTOR vNormal = XMLoadFloat3(&Hit.Normal);
			XMVECTOR vPush = vNormal * Hit.Depth;
			XMVECTOR vCurrentPos = XMLoadFloat3(&Player_Pos);
			vCurrentPos += vPush;
			XMStoreFloat3(&Player_Pos, vCurrentPos);

			// Landed on Object
			if (Hit.Normal.y > 0.5f) 
			{
				Vel = XMVectorSetY(Vel, 0.0f);
				Player_Vel.y = 0.0f;
				Is_Jump = false;
			}
			// Hit Head
			else if (Hit.Normal.y < -0.5f)
			{
				Vel = XMVectorSetY(Vel, 0.0f);
				Player_Vel.y = 0.0f;
			}

			// Re-calc AABB
			Vertical_AABB = Player_Get_AABB();
			Vertical_AABB.Min.x += skinWidth; Vertical_AABB.Max.x -= skinWidth;
			Vertical_AABB.Min.z += skinWidth; Vertical_AABB.Max.z -= skinWidth;
		}
	}

	// XZ Axis Collision
	for (int i = 0; i < OBJ_Count; i++)
	{
		const MapObject* OBJ = Map_System_Get_Object(i);
		if (OBJ->OBJ_ID == FIELD)
			continue;

		IsHit Hit = Collision_Is_Hit_AABB(Horizontal_AABB, OBJ->Collision);

		// XZ Axis Collision
		if (Hit.Is_Hit && Hit.Normal.y == 0.0f)
		{
			// Push Out
			XMVECTOR vNormal = XMLoadFloat3(&Hit.Normal);
			XMVECTOR vPush = vNormal * Hit.Depth;
			XMVECTOR vCurrentPos = XMLoadFloat3(&Player_Pos);
			vCurrentPos += vPush;
			XMStoreFloat3(&Player_Pos, vCurrentPos);

			// Wall Sliding
			float velocityDot = XMVectorGetX(XMVector3Dot(Vel, vNormal));
			if (velocityDot < 0.0f)
			{
				Vel -= vNormal * velocityDot;
			}

			// AABB Update
			Horizontal_AABB = Player_Get_AABB();
		}
	}

	float Half_Size_X = Mash_Field_Get_Size_X() * 0.5f;
	float Half_Size_Z = Mash_Field_Get_Size_Z() * 0.5f;

	bool warped = false;

	if (Player_Pos.x > Half_Size_X)
	{
		Player_Pos.x -= Mash_Field_Get_Size_X();
		warped = true;
	}
	else if (Player_Pos.x < -Half_Size_X)
	{
		Player_Pos.x += Mash_Field_Get_Size_X();
		warped = true;
	}

	if (Player_Pos.z > Half_Size_Z)
	{
		Player_Pos.z -= Mash_Field_Get_Size_Z();
		warped = true;
	}
	else if (Player_Pos.z < -Half_Size_Z)
	{
		Player_Pos.z += Mash_Field_Get_Size_Z();
		warped = true;
	}

	// Save Final Front & Vel
	XMStoreFloat3(&Player_Front, Flat_Front);
	XMStoreFloat3(&Player_Vel, Vel);
}

void Player_Draw()
{
	if (!Player_Model)
		return;

	Shader_Manager::GetInstance()->SetLightSpecular(Player_Camera_Get_POS(), 164.0f, { 1.0f, 1.0f, 1.0f, 1.0f });

	// Get Movement
	XMMATRIX t = XMMatrixTranslation(Player_Pos.x, Player_Pos.y + Player_Height_Offset, Player_Pos.z);
	
	// Get Yaw
	float yaw = atan2f(Player_Front.x, Player_Front.z);

	// Reverse Player
	float modelYaw = yaw + XM_PI;
	XMMATRIX r = XMMatrixRotationY(modelYaw);
	XMMATRIX world = r * t;

	ModelDraw(Player_Model, world);

	if (Get_Debug_Mode_State())
	{
		Debug_Collision_Draw(Player_Get_AABB(), Red);
	}
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

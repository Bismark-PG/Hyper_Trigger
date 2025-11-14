/*==============================================================================

	 Make Bullet [Bullet.cpp]

	 Author : Choi HyungJoon

==============================================================================*/
#include "Bullet.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "direct3d.h"
#include "Cube.h"
#include "KeyLogger.h"
#include "Player.h"
#include "Player_Camera.h"
#include <Model.h>
#include <Game_Model.h>

class Bullet
{
public:
	Bullet(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& vel)
		: Position(pos), Velocity(vel)
	{
	};

	void Update(double elapsed_time)
	{
		Accumulated_time += elapsed_time;
		DirectX::XMStoreFloat3(&Position, DirectX::XMLoadFloat3(&Position)
			+ DirectX::XMLoadFloat3(&Velocity)
			* static_cast<float>(elapsed_time));
	}

	const DirectX::XMFLOAT3& Get_POS() const
	{
		return Position;
	}

	DirectX::XMFLOAT3 Get_Front() const
	{
		DirectX::XMFLOAT3 Front;
		DirectX::XMStoreFloat3(&Front, DirectX::XMVector3Normalize(XMLoadFloat3(&Velocity)));
		return Front;
	}

	bool Is_Destroy() const
	{
		return Accumulated_time >= TIME_LIMIT;
	}

private:
	DirectX::XMFLOAT3 Position{};
	DirectX::XMFLOAT3 Velocity{};
	double Accumulated_time{ 0.0 };
	static constexpr double TIME_LIMIT = 3.0;
	bool Is_Destroyed = false;
};

static Bullet* Bullets[MAX_BULLET]{};
int Bullets_Count = 0;

static float g_X = 0.0f;
static float g_Angle = 0.0f;
static float g_Scale = 1.0f;
static double AccumuTime = 0.0f;

static XMFLOAT3 Cube_POS{};
static XMFLOAT3 Cube_Vel{};

static MODEL* M_Bullet = nullptr;

void Bullet_Initialize()
{
	M_Bullet = Model_M->GetModel("ball");
}

void Bullet_Finalize()
{
	for (int i = 0; i < MAX_BULLET; i++)
	{
		delete Bullets[i];
	}

	Bullets_Count = 0;
}

void Bullet_Update(double elapsed_time)
{
	AccumuTime += elapsed_time;

	for (int i = 0; i < Bullets_Count; i++)
	{
		if (Bullets[i]->Is_Destroy())
		{
			Bullet_Destroy(i);
		}
	}

	for (int i = 0; i < Bullets_Count; i++)
	{
		Bullets[i]->Update(AccumuTime);
	}

	g_X = static_cast<float>(sin(AccumuTime)) * 3.5f;
	g_Angle = static_cast<float>(AccumuTime * 90.0f);
	g_Scale = static_cast<float>(sin(AccumuTime * 3.0f));

	if (KeyLogger_IsMouseTrigger(Mouse_Button::LEFT))
	{
		Cube_POS = { Player_Get_POS().x, Player_Get_POS().y + 1.5f, Player_Get_POS().z };
		XMStoreFloat3(&Cube_Vel, XMLoadFloat3(&Player_Camera_Get_Front()) * 10.0f);
	}

	XMVECTOR Cube_Position = XMLoadFloat3(&Cube_POS);
	Cube_Position += XMLoadFloat3(&Cube_Vel) * static_cast<float>(elapsed_time);
	XMStoreFloat3(&Cube_POS, Cube_Position);
}

void Bullet_Draw()
{
	XMMATRIX mtxWorld = {};

	for (int i = 0; i < Bullets_Count; i++)
	{
		XMVECTOR POS = XMLoadFloat3(&Bullets[i]->Get_POS());
		mtxWorld = XMMatrixTranslationFromVector(POS);
		ModelDraw(M_Bullet, mtxWorld);
	}

	XMMATRIX MTX_Cube_Throw = XMMatrixRotationY(XMConvertToRadians(g_Angle));
	MTX_Cube_Throw = XMMatrixTranslationFromVector(XMLoadFloat3(&Cube_POS));
	Cube_Draw(MTX_Cube_Throw, {}, Cube_Type::SAMPLE);
}

void Bullet_Create(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity)
{
	if (Bullets_Count >= MAX_BULLET)
	{
		// D_Out::debug message
		return;
	}

	Bullets[Bullets_Count++] = new Bullet(position, velocity);
}

void Bullet_Destroy(int Index)
{
	delete Bullets[Index];
	Bullets[Index] = Bullets[Bullets_Count - 1];
	Bullets_Count--;
}

int Bullet_Get_Bullets_Count()
{
	return Bullets_Count;
}

AABB Bullet_Get_AABB(int index)
{
	return Model_Get_AABB(M_Bullet, Bullets[index]->Get_POS());
}

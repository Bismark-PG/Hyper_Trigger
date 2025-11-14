/*==============================================================================

	Make Debug Cube [Debug_Camera.cpp]
	
	Author : Choi HyungJoon

==============================================================================*/

#include "Debug_Camera.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "KeyLogger.h"
#include "Shader_Manager.h"
#include "Player_Camera.h"

static float Debug_Camera_Yaw = 0.0f;
static float Debug_Camera_Pitch = 0.0f;

static XMFLOAT3 Debug_Camera_POS		= { 0.0f, 0.0f, -5.0f };
static XMFLOAT3 Debug_Camera_Front		= { 0.0f, 0.0f, 1.0f };
static XMFLOAT3 Debug_Camera_Vertical	= { 0.0f, 1.0f, 0.0f };
static XMFLOAT3 Debug_Camera_Horizon	= { 1.0f, 0.0f, 0.0f };
static constexpr float Debug_Camera_Move_Speed		= 3.0f;
static constexpr float Debug_Camera_Rotate_Speed	= XMConvertToRadians(60.0f);

static constexpr XMVECTOR World_Vertical{ 0.0f, 1.0f, 0.0f };
static constexpr XMVECTOR World_Horizon	{ 1.0f, 0.0f, 1.0f };

static float Debug_Camera_FOV = XMConvertToRadians(60.0f);
static float Debug_Camera_FOV_Move = 45.0f;

static XMFLOAT4X4 Debug_Camera_Mat = {};
static XMFLOAT4X4 Debug_Camera_Per_Mat = {};

void Debug_Camera_Initialize(const XMFLOAT3& POS, const XMFLOAT3& Front, const XMFLOAT3& UP, const XMFLOAT3& Right)
{
	Debug_Camera_POS		= POS;
	XMStoreFloat3(&Debug_Camera_Front,	 XMVector3Normalize(XMLoadFloat3(&Front)));
	XMStoreFloat3(&Debug_Camera_Vertical,	 XMVector3Normalize(XMLoadFloat3(&UP)));
	XMStoreFloat3(&Debug_Camera_Horizon,	 XMVector3Normalize(XMLoadFloat3(&Right)));

	Debug_Camera_Initialize();
}

void Debug_Camera_Initialize()
{
	Debug_Camera_FOV = XMConvertToRadians(60.0f);
	Debug_Camera_FOV_Move = 45.0f;

	XMStoreFloat4x4(&Debug_Camera_Mat, XMMatrixIdentity());
	XMStoreFloat4x4(&Debug_Camera_Per_Mat, XMMatrixIdentity());
}

void Debug_Camera_Finalize()
{
}

void Debug_Camera_Update(double elapsed_time)
{
	float Time = static_cast<float>(elapsed_time);

	XMVECTOR POS	  = XMLoadFloat3(&Debug_Camera_POS);
	XMVECTOR Front	  = XMLoadFloat3(&Debug_Camera_Front);
	XMVECTOR Vertical = XMLoadFloat3(&Debug_Camera_Vertical);
	XMVECTOR Horizon  = XMLoadFloat3(&Debug_Camera_Horizon);

	// --- Camera Movement (WASD, Space, Shift) ---
	// Move Forward And Back
	if (KeyLogger_IsPressed(KK_W))
		POS += Front * Debug_Camera_Move_Speed * Time;
	if (KeyLogger_IsPressed(KK_S))
		POS -= Front * Debug_Camera_Move_Speed * Time;
	
	// Turn L And R
	if (KeyLogger_IsPressed(KK_A))
		POS -= Horizon * Debug_Camera_Move_Speed * Time;
	if (KeyLogger_IsPressed(KK_D))
		POS += Horizon * Debug_Camera_Move_Speed * Time;

	// UP and DOWN
	if (KeyLogger_IsPressed(KK_SPACE))
	{
		// Move POS Based Camera POS
		// POS += Vertical * Camera_Move_Speed * Time;

		// Move POS Based World POS
		POS += World_Vertical * Debug_Camera_Move_Speed * Time;
	}
	if (KeyLogger_IsPressed(KK_LEFTSHIFT))
	{
		// Move POS Based Camera POS
		// POS -= Vertical * Camera_Move_Speed * Time;

		// Move POS Based World POS
		POS -= World_Vertical * Debug_Camera_Move_Speed * Time;
	}

	// FOV UP and DOWN // Pitch
	if (KeyLogger_IsPressed(KK_UP))
	{
		XMMATRIX Rotate = XMMatrixRotationAxis(Horizon, -Debug_Camera_Rotate_Speed * Time);
		Front = XMVector3TransformNormal(Front, Rotate);
		Front = XMVector3Normalize(Front);
		Vertical = XMVector3Cross(Front, Horizon);
	}
	if (KeyLogger_IsPressed(KK_DOWN))
	{
		XMMATRIX Rotate = XMMatrixRotationAxis(Horizon, Debug_Camera_Rotate_Speed * Time);
		Front = XMVector3TransformNormal(Front, Rotate);
		Front = XMVector3Normalize(Front);
		Vertical = XMVector3Cross(Front, Horizon);
	}

	// FOV UP and DOWN // Yaw
	if (KeyLogger_IsPressed(KK_LEFT))
	{
		// Move POS Based Camera POS
		// XMMATRIX Rotate = XMMatrixRotationAxis(Vertical, -Camera_Rotate_Speed * Time);

		// Move POS Based World POS
		XMMATRIX Rotate = XMMatrixRotationY(-Debug_Camera_Rotate_Speed * Time);
		Vertical = XMVector3Normalize(XMVector3TransformNormal(Vertical, Rotate));

		Front = XMVector3TransformNormal(Front, Rotate);
		Front = XMVector3Normalize(Front);
		Horizon = XMVector3Cross(Vertical, Front) * World_Horizon;
	}
	if (KeyLogger_IsPressed(KK_RIGHT))
	{
		// Move POS Based Camera POS
		// XMMATRIX Rotate = XMMatrixRotationAxis(Vertical, Camera_Rotate_Speed * Time);

		// Move POS Based World POS
		XMMATRIX Rotate = XMMatrixRotationY(Debug_Camera_Rotate_Speed * Time);
		Vertical = XMVector3Normalize(XMVector3TransformNormal(Vertical, Rotate));

		Front = XMVector3TransformNormal(Front, Rotate);
		Front = XMVector3Normalize(Front);
		Horizon = XMVector3Cross(Vertical, Front) * World_Horizon;
	}

	// Control FOV (Zoom In, Out)
	if (KeyLogger_IsPressed(KK_Z))
		Debug_Camera_FOV -= XMConvertToRadians(Debug_Camera_FOV_Move) * Time;
	if (KeyLogger_IsPressed(KK_C))
		Debug_Camera_FOV += XMConvertToRadians(Debug_Camera_FOV_Move) * Time;


	// Save Vectors and Create Matrices
	XMStoreFloat3(&Debug_Camera_POS, POS);
	XMStoreFloat3(&Debug_Camera_Front, Front);
	XMStoreFloat3(&Debug_Camera_Vertical, Vertical);
	XMStoreFloat3(&Debug_Camera_Horizon, Horizon);

	//Create the view matrix
	XMMATRIX View = XMMatrixLookAtLH(
		POS,			// POS
		POS + Front,	// FOV
		Vertical);		// UP Level
	XMStoreFloat4x4(&Debug_Camera_Mat, View);

	// Create the projection matrix
	float Ratio = static_cast<float>(Direct3D_GetBackBufferWidth()) / Direct3D_GetBackBufferHeight();
	float Near_Z = 0.1f;
	float Far_Z = 100.0f;
	XMMATRIX mtxPerspective = XMMatrixPerspectiveFovLH(Debug_Camera_FOV, Ratio, Near_Z, Far_Z);

	XMStoreFloat4x4(&Debug_Camera_Per_Mat, mtxPerspective);
	// Set View MAT And Projection MAT In Shader
	Shader_M->SetViewMatrix3D(View);
	Shader_M->SetProjectionMatrix3D(mtxPerspective);

	/*
	// --- Note: Simplified Rotation Method ---
	// This method is shorter but can introduce slight roll over time if not re-orthogonalized.
	// It works by assuming the camera's axes form a rigid body and rotating them all together.
	
	// Simplified Pitch:
	// XMMATRIX rotation = XMMatrixRotationAxis(right, angle * dt);
	// front = XMVector3TransformNormal(front, rotation);
	// up    = XMVector3TransformNormal(up, rotation);
	
	// Simplified Yaw:
	// XMMATRIX rotation = XMMatrixRotationAxis(worldUp, angle * dt);
	// front = XMVector3TransformNormal(front, rotation);
	// right = XMVector3TransformNormal(right, rotation);
	
	// To make the simplified method stable, you would re-orthogonalize at the end of the update:
	// front = XMVector3Normalize(front);
	// right = XMVector3Normalize(XMVector3Cross(worldUp, front));
	// up    = XMVector3Cross(front, right);
	*/
}

const XMFLOAT4X4& Debug_Camera_Get_Matrix()
{
	return Debug_Camera_Mat;
}

const XMFLOAT4X4& Debug_Camera_Get_Perspective_Matrix()
{
	return Debug_Camera_Per_Mat;
}

const XMFLOAT3& Debug_Camera_Get_POS()
{
	return Debug_Camera_POS;
}

const XMFLOAT3& Debug_Camera_Get_Front()
{
	return Debug_Camera_Front;
}

const DirectX::XMFLOAT3& Debug_Camera_Get_Vertical()
{
	return Debug_Camera_Vertical;
}

const DirectX::XMFLOAT3& Debug_Camera_Get_Horizon()
{
	return Debug_Camera_Horizon;
}

float Debug_Camera_Get_FOV()
{
	return Debug_Camera_FOV;
}

void Debug_Camera_Set_Position(float X, float Y, float Z)
{
	Debug_Camera_POS = { X, Y, Z };
}

void Debug_Camera_Set_Position(const DirectX::XMFLOAT3& POS)
{
	Debug_Camera_POS = POS;
}

void Debug_Camera_Set_Rotation(float Y, float P)
{
	Debug_Camera_Yaw = Y;
	Debug_Camera_Pitch = P;
}
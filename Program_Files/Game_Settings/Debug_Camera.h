/*==============================================================================

	Make Debug Cube [Debug_Camera.h]
	
	Author : Choi HyungJoon

==============================================================================*/
#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>

void Debug_Camera_Initialize(const DirectX::XMFLOAT3& POS,
	const DirectX::XMFLOAT3& Front	= { 0.0f, 0.0f, 1.0f },
	const DirectX::XMFLOAT3& Right	= { 0.0f, 1.0f, 0.0f },
	const DirectX::XMFLOAT3& UP		= { 1.0f, 0.0f, 0.0f });

void Debug_Camera_Initialize();
void Debug_Camera_Finalize();

void Debug_Camera_Update(double elapsed_time);

const DirectX::XMFLOAT4X4& Debug_Camera_Get_Matrix();
const DirectX::XMFLOAT4X4& Debug_Camera_Get_Perspective_Matrix();

const DirectX::XMFLOAT3& Debug_Camera_Get_POS();
const DirectX::XMFLOAT3& Debug_Camera_Get_Front();
const DirectX::XMFLOAT3& Debug_Camera_Get_Vertical();
const DirectX::XMFLOAT3& Debug_Camera_Get_Horizon();
float Debug_Camera_Get_FOV();

void Debug_Camera_Set_Position(float X, float Y, float Z);
void Debug_Camera_Set_Position(const DirectX::XMFLOAT3& POS);
void Debug_Camera_Set_Rotation(float Y, float P);

#endif //CAMERA_H
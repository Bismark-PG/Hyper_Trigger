/*==============================================================================
	
	Reset Direct3D [direct3d.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef DIRECT3D_H
#define DIRECT3D_H


#include <Windows.h>
#include <d3d11.h>

// セーフリリースマクロ
#define SAFE_RELEASE(o) if (o) { (o)->Release(); o = NULL; }

bool Direct3D_Initialize(HWND hWnd); // Direct3Dの初期化
void Direct3D_Finalize(); // Direct3Dの終了処理

void Direct3D_Clear(); // バックバッファのクリア
void Direct3D_Present(); // バックバッファの表示

void Direct3D_Re_Size(UINT width, UINT height);

unsigned int Direct3D_GetBackBufferWidth();
unsigned int Direct3D_GetBackBufferHeight();

// Earn D3D Device
ID3D11Device* Direct3D_GetDevice();
// Earn D3D Context
ID3D11DeviceContext* Direct3D_GetContext();

void Direct3D_SetDepthEnable(bool enable);

#endif // DIRECT3D_H

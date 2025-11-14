/*==============================================================================

	Manage System Logic [System_Logic_Manager.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef SYSTEM_LOGIC_MANAGER
#define SYSTEM_LOGIC_MANAGER
#include "Game_Header_Manager.h"

void System_Initialize(HWND hWnd, ID3D11Device* Device, ID3D11DeviceContext* Context);
void System_Finalize();

#endif // SYSTEM_LOGIC_MANAGER
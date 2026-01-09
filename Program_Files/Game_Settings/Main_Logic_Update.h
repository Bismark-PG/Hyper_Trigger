/*==============================================================================

	Update Manager Title And Main UI [Main_UI_Setting.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef MAIN_LOGIC_H
#define MAIN_LOGIC_H
#include <d3d11.h>

void Main_Logic_Initialize(ID3D11Device* Device, ID3D11DeviceContext* Context);
void Main_Logic_Finalize();

void Main_Logic_Update(double elapsed_time);
#endif // MAIN_LOGIC_H
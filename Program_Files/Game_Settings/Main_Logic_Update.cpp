/*==============================================================================

	Update Manager Title And Main UI [Main_UI_Setting.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Main_Logic_Update.h"
#include "Fade.h"
#include "Title.h"     
#include "Game.h"
#include "Mash_Field.h"
#include "Main_Menu.h"
#include "Game_Screen_Manager.h"

void Main_Logic_Initialize(ID3D11Device* Device, ID3D11DeviceContext* Context)
{
	Title_Initialize();
	Main_Menu_Initialize();
	Game_Initialize(Device, Context);
}

void Main_Logic_Finalize()
{
	Game_Finalize();
	Main_Menu_Finalize();
	Title_Finalize();
}

void Main_Logic_Update(double elapsed_time)
{
}
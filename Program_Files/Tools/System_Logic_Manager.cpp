/*==============================================================================

	Manage System Logic [System_Logic_Manager.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "System_Logic_Manager.h"

void System_Initialize(HWND hWnd, ID3D11Device* Device, ID3D11DeviceContext* Context)
{
	// Initialize System Tools
	SystemTimer_Initialize();
	KeyLogger_Initialize(hWnd);

	// Initialize Shader Tools
	Shader_M = Shader_Manager::GetInstance();
	Shader_M->Init(Device, Context);

	// Initialize Texture Tools
	Texture_M = Texture_Manager::GetInstance();
	Texture_M->Init(Device, Context);
	Game_Texture_Initialize();
	Model_M->Init(Device, Context);
	Sprite_Initialize(Device, Context);
	SpriteAni_Initialize();

	// Initialize Audio Tools
	Sound_M = Audio_Manager::GetInstance();
	Sound_M->Init();
	Game_Audio_Initialize();

	// Initialize Game Setting Tools
	Game_Initialize(Device, Context);
	Mash_Field_Initialize(Device, Context);
}

void System_Finalize()
{
	Mash_Field_Finalize();
	SpriteAni_Finalize();
	Sprite_Finalize();
	Model_M->Final();
	Game_Texture_Finalize();
	Texture_M->Final();
	Shader_M->Final();
	Direct3D_Finalize();
	Mouse_Finalize();

}
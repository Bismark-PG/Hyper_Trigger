/*==============================================================================

	Manage Game Audio Resource [Game_Audio.h]

	Author : Choi HyungJoon

--------------------------------------------------------------------------------
	- Reference

		¡ BGM ¡

		Ÿ DOVA-SYNDROME
		> https://dova-s.jp/

		¡ SFX ¡

		Ÿ  Sound Effect Lab (Œø‰Ê‰¹ƒ‰ƒ{)
		> https://soundeffect-lab.info/

==============================================================================*/
#include "Game_Audio.h"
#include "Audio_Manager.h"

void Game_Audio_Initialize()
{
	Game_Audio_BGM();
	Game_Audio_SFX();
}

void Game_Audio_Finalize()
{
}

void Game_Audio_BGM()
{
	// Sound_M->Load_BGM("Title", "Resource/BGM/Title_Ancient_memories.wav");
}

void Game_Audio_SFX()
{
	// Sound_M->Load_SFX("Logo_Draw", "Resource/SFX/Logo_Draw_Fixed.wav");
}
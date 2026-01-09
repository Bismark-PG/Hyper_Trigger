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

Audio_Manager* Sound_M;

void Game_Audio_Initialize()
{
	Sound_M = Audio_Manager::GetInstance();

	Game_Audio_BGM();
	Game_Audio_SFX();
}

void Game_Audio_Finalize()
{
}

void Game_Audio_BGM()
{
	Sound_M->Load_BGM("Title", "Resource/BGM/Assignment-1.wav");
}

void Game_Audio_SFX()
{
	Sound_M->Load_SFX("Logo_Draw", "Resource/SFX/Logo_Draw_Fixed.wav");

	Sound_M->Load_SFX("Buffer_Move", "Resource/SFX/Buffer_Move.wav");
	Sound_M->Load_SFX("Buffer_Select", "Resource/SFX/Buffer_Select.wav");
	Sound_M->Load_SFX("Buffer_Back", "Resource/SFX/Buffer_Back.wav");
	Sound_M->Load_SFX("Buffer_Denied", "Resource/SFX/Buffer_Denied.wav");
	Sound_M->Load_SFX("Controller_Alert", "Resource/SFX/Controller_Alert.wav");
}
/*==============================================================================

	Update Game Screen [Update_Game.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Update_Game.h"
#include "debug_ostream.h"
#include "System_Timer.h"
#include "Fade.h"
#include "Title.h"     
#include "Game.h"
#include "Main_Menu.h"

void Main_Game_Screen_Update()
{
	Main_Screen current_screen = Game_Manager::GetInstance()->Get_Current_Main_Screen();

	switch (current_screen)
	{
	case Main_Screen::M_WAIT:
		Fade_Start(1.5f, false);
		Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::MAIN);
		break;

	case Main_Screen::MAIN:
		Title_Draw();
		break;

	case Main_Screen::MENU_SELECT:
		Main_Menu_Draw();
		//Game_Draw();
		break;

	case Main_Screen::SELECT_GAME:
		Game_Draw();
		break;

	case Main_Screen::SELECT_SETTINGS:
		Game_Manager::GetInstance()->Update_Sub_Screen(Sub_Screen::SETTINGS);
		break;

	case Main_Screen::EXIT:
		Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::M_DONE);
		Debug::D_Out << "Exiting Game..." << std::endl;
		break;

	case Main_Screen::M_DONE:
		Debug::D_Out << "Programme Done" << std::endl;
		break;
	}
}

void Sub_Game_Screen_Update()
{
	Sub_Screen current_screen = Game_Manager::GetInstance()->Get_Current_Sub_Screen();

	switch (current_screen)
	{
	case Sub_Screen::S_WAIT:
		if (Game_Manager::GetInstance()->Get_Current_Main_Screen() == Main_Screen::SELECT_SETTINGS)
			Game_Manager::GetInstance()->Update_Sub_Screen(Sub_Screen::SETTINGS);
		break;

	case Sub_Screen::SETTINGS:
		break;

	case Sub_Screen::S_DONE:
		Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::MENU_SELECT);
		break;
	}
}

void Game_Select_Screen_Update()
{
	Game_Select_Screen current_screen = Game_Manager::GetInstance()->Get_Current_Game_Select_Screen();

	switch (current_screen)
	{
	case Game_Select_Screen::G_WAIT:
		if (Game_Manager::GetInstance()->Get_Current_Main_Screen() == Main_Screen::SELECT_GAME)
			Game_Manager::GetInstance()->Update_Game_Select_Screen(Game_Select_Screen::GAME_MENU_SELECT);
		break;

	case Game_Select_Screen::GAME_MENU_SELECT:
		// Not Use Now
		break;

	case Game_Select_Screen::GAME_PLAYING:
		break;

	case Game_Select_Screen::G_DONE:
		Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::MENU_SELECT);
		break;
	}
}

void Game_Screen_Draw()
{
	Main_Game_Screen_Update();
	Sub_Game_Screen_Update();
	Game_Select_Screen_Update();
}

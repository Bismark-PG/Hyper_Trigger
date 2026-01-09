/*==============================================================================

	Game Main Menu [Main_Menu.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef MAIN_MENU_H
#define MAIN_MENU_H

enum class Main_Select_Buffer
{
	None,
	Start,
	Setting,
	Exit,
	Done
};

struct Button_RECT
{
	float x, y, w, h;
};

void Main_Menu_Initialize();
void Main_Menu_Finalize();

void Main_Menu_Update(double elapsed_time);
void Main_Menu_Draw();

Main_Select_Buffer Get_Main_Menu_Buffet();
void Set_Main_Menu_Buffer(Main_Select_Buffer Buffer);

bool Is_Mouse_In_RECT(float mx, float my, float bx, float by, float bw, float bh);
bool IF_IS_Game_Done();

void Main_Menu_Texture();

#endif // MAIN_MENU_H
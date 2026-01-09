/*==============================================================================

	Game Main Menu [Main_Menu.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Main_Menu.h"
#include "Map_System.h"
#include "Fade.h"
#include "Audio_Manager.h"
#include "Game_Screen_Manager.h"
#include "Shader_Manager.h"
#include "Sky.h"
#include "mouse.h"           
#include "Texture_Manager.h"
#include "KeyLogger.h"
#include "Controller_Input.h"

int Mouse_UI = -1;
int UI_Start = -1;
int UI_Set = -1;
int UI_Exit = -1;

Main_Select_Buffer M_Buffer;

static float Current_Volume = 0.0f;
static double BGM_Fade_Timer = 0.0;
constexpr double BGM_FADE_TIME = 3.0;
bool Is_BGM_Playing;
bool Is_BGM_Fading_In;

float UI_X		 = -1;
float Start_Y	 = -1;
float Settings_Y = -1;
float Exit_Y	 = -1;
float UI_Width	 = -1;
float UI_Height	 = -1;

static float Cursor_X = 0.0f;
static float Cursor_Y = 0.0f;
static float Cursor_Size = 0.0f;
static bool Is_Mouse_Left_Clicked_Prev = false;

static bool EXIT_STATE = false;
static constexpr double FADE_OUT_TIME = 2.0;
static double Fade_Out_Timer = 0.0;

static bool Controller_Alert = false;
static bool Wait_For_Release = false;

void Main_Menu_Initialize()
{
	Main_Menu_Texture();
	Mouse_SetVisible(false);

	float Screen_W = static_cast<float>(Direct3D_GetBackBufferWidth());
	float Screen_H = static_cast<float>(Direct3D_GetBackBufferHeight());

	Cursor_Size = Screen_H * 0.025f;

	UI_Width	= Screen_W * 0.15f;
	UI_Height	= Screen_H * 0.05f;
	UI_X		= Screen_W * 0.15f - UI_Width * 0.5f;
	Start_Y		= Screen_H * 0.6f - Start_Y * 0.5f;
	Settings_Y	= Screen_H * 0.7f - Settings_Y * 0.5f;
	Exit_Y		= Screen_H * 0.8f - Exit_Y * 0.5f;

	M_Buffer = Main_Select_Buffer::None;

	Is_BGM_Playing = false;
	Is_BGM_Fading_In = false;
}

void Main_Menu_Finalize()
{
}

void Main_Menu_Update(double elapsed_time)
{
	float dt = static_cast<float>(elapsed_time);

	bool Controller_Alert_Now = Controller_Set_UP();

	if (Controller_Alert_Now)
	{
		Controller_Alert = true;
		Wait_For_Release = true;
		return;
	}

	if (Wait_For_Release)
	{
		if (KeyLogger_IsAnyKeyReleased() || XKeyLogger_IsAnyPadReleased())
			Wait_For_Release = false;
		return;
	}

	Controller_Alert = false;

	if (!Is_BGM_Playing && Game_Manager::GetInstance()->Get_Current_Main_Screen() == Main_Screen::MENU_SELECT)
	{
		if (Fade_GetState() == FADE_STATE::FINISHED_IN)
		{
			Debug::D_Out << "[Title] Main Title Draw" << std::endl;

			Audio_Manager::GetInstance()->Play_BGM("Title", true);
			Audio_Manager::GetInstance()->Update_Current_BGM_Volume(0.0f);

			Is_BGM_Playing = true;
			Is_BGM_Fading_In = true;
			BGM_Fade_Timer = 0.0;
		}
	}

	if (Is_BGM_Fading_In && BGM_Fade_Timer < BGM_FADE_TIME)
	{
		BGM_Fade_Timer += dt;
		float Target_Volume = Audio_Manager::GetInstance()->Get_Target_BGM_Volume();
		Current_Volume = Target_Volume * static_cast<float>(BGM_Fade_Timer / BGM_FADE_TIME);

		if (Current_Volume > Target_Volume)
		{
			Current_Volume = Target_Volume;
			Is_BGM_Fading_In = false;
		}

		Audio_Manager::GetInstance()->Update_Current_BGM_Volume(Current_Volume);
	}

	Mouse_State mState;
	Mouse_GetState(&mState);

	Cursor_X = (float)mState.x;
	Cursor_Y = (float)mState.y;

	bool Is_Clicked = mState.leftButton && !Is_Mouse_Left_Clicked_Prev;
	Is_Mouse_Left_Clicked_Prev = mState.leftButton;

	if (Is_Mouse_In_RECT(Cursor_X, Cursor_Y, UI_X, Start_Y, UI_Width, UI_Height))
	{
		Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
		Set_Main_Menu_Buffer(Main_Select_Buffer::Start);
		if (Is_Clicked)
		{
			Audio_Manager::GetInstance()->Play_SFX("Buffer_Select");
			Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::SELECT_GAME);
		}
	}
	else if (Is_Mouse_In_RECT(Cursor_X, Cursor_Y, UI_X, Settings_Y, UI_Width, UI_Height))
	{
		Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
		Set_Main_Menu_Buffer(Main_Select_Buffer::Setting);
		if (Is_Clicked)
		{
			Audio_Manager::GetInstance()->Play_SFX("Buffer_Select");
			Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::SELECT_SETTINGS);
		}
	}
	else if (Is_Mouse_In_RECT(Cursor_X, Cursor_Y, UI_X, Exit_Y, UI_Width, UI_Height))
	{
		Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
		Set_Main_Menu_Buffer(Main_Select_Buffer::Exit);
		if (Is_Clicked)
		{
			Audio_Manager::GetInstance()->Play_SFX("Buffer_Select");
			Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::EXIT);
		}
	}

	switch (M_Buffer)
	{
	case Main_Select_Buffer::None:
		if (KeyLogger_IsTrigger(KK_W) || KeyLogger_IsTrigger(KK_UP) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_UP))
		{
			Set_Main_Menu_Buffer(Main_Select_Buffer::Exit);
			Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
		}
		else if (KeyLogger_IsTrigger(KK_S) || KeyLogger_IsTrigger(KK_DOWN) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_DOWN))
		{
			Set_Main_Menu_Buffer(Main_Select_Buffer::Start);
			Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
		}
		break;

	case Main_Select_Buffer::Start:
		if (KeyLogger_IsTrigger(KK_ENTER) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_A))
		{
			Set_Main_Menu_Buffer(Main_Select_Buffer::Done);
			Audio_Manager::GetInstance()->Play_SFX("Buffer_Select");
		}

		if (KeyLogger_IsTrigger(KK_S) || KeyLogger_IsTrigger(KK_DOWN) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_DOWN))
		{
			Set_Main_Menu_Buffer(Main_Select_Buffer::Setting);
			Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
		}
		break;

	case Main_Select_Buffer::Setting:
		if (KeyLogger_IsTrigger(KK_ENTER) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_A))
		{
			Set_Main_Menu_Buffer(Main_Select_Buffer::Done);
			Audio_Manager::GetInstance()->Play_SFX("Buffer_Select");
		}

		if (KeyLogger_IsTrigger(KK_W) || KeyLogger_IsTrigger(KK_UP) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_UP))
		{
			Set_Main_Menu_Buffer(Main_Select_Buffer::Start);
			Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
		}
		else if (KeyLogger_IsTrigger(KK_S) || KeyLogger_IsTrigger(KK_DOWN) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_DOWN))
		{
			Set_Main_Menu_Buffer(Main_Select_Buffer::Exit);
			Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
		}
		break;

	case Main_Select_Buffer::Exit:
		if (KeyLogger_IsTrigger(KK_ENTER) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_A))
		{
			Set_Main_Menu_Buffer(Main_Select_Buffer::Done);
			Fade_Start(FADE_OUT_TIME, true);
			EXIT_STATE = true;
			Fade_Out_Timer = 0.0;

			Audio_Manager::GetInstance()->Play_SFX("Buffer_Select");
		}

		if (KeyLogger_IsTrigger(KK_W) || KeyLogger_IsTrigger(KK_UP) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_UP))
		{
			Set_Main_Menu_Buffer(Main_Select_Buffer::Setting);
			Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
		}
		break;

	case Main_Select_Buffer::Done:
		break;
	}
}

void Main_Menu_Draw()
{
	// Game Demo Draw
	Direct3D_SetDepthEnable(true);
	Shader_Manager::GetInstance()->Begin3D();

	// UI
	Direct3D_SetDepthEnable(false);
	Shader_Manager::GetInstance()->Begin2D();

	if (M_Buffer == Main_Select_Buffer::Start)
		Sprite_Draw(UI_Start, UI_X, Start_Y, UI_Width, UI_Height);
	else
		Sprite_Draw(UI_Start, UI_X, Start_Y, UI_Width, UI_Height, A_Zero, { A_Origin, A_Origin, A_Origin, A_Half });

	if (M_Buffer == Main_Select_Buffer::Setting)
		Sprite_Draw(UI_Set, UI_X, Settings_Y, UI_Width, UI_Height);
	else
		Sprite_Draw(UI_Set, UI_X, Settings_Y, UI_Width, UI_Height, A_Zero, { A_Origin, A_Origin, A_Origin, A_Half });

	if (M_Buffer == Main_Select_Buffer::Exit)
		Sprite_Draw(UI_Exit, UI_X, Exit_Y, UI_Width, UI_Height);
	else
		Sprite_Draw(UI_Exit, UI_X, Exit_Y, UI_Width, UI_Height, A_Zero, { A_Origin, A_Origin, A_Origin, A_Half });

	// Mouse
	Sprite_Draw(Mouse_UI, Cursor_X, Cursor_Y, Cursor_Size, Cursor_Size);
}

Main_Select_Buffer Get_Main_Menu_Buffet()
{
	return M_Buffer;
}

void Set_Main_Menu_Buffer(Main_Select_Buffer Buffer)
{
	M_Buffer = Buffer;
}

bool Is_Mouse_In_RECT(float mx, float my, float bx, float by, float bw, float bh)
{
	return (mx >= bx && mx <= bx + bw && my >= by && my <= by + bh);
}

bool IF_IS_Game_Done()
{
	return EXIT_STATE;
}

void Main_Menu_Texture()
{
	//---------------Intro Logo Texture---------------//
	Mouse_UI = Texture_Manager::GetInstance()->GetID("K");

	UI_Start = Texture_Manager::GetInstance()->GetID("Start");
	UI_Set	 = Texture_Manager::GetInstance()->GetID("Settings");
	UI_Exit  = Texture_Manager::GetInstance()->GetID("Exit");

	if (Mouse_UI == -1 || UI_Start == -1 || UI_Set == -1 || UI_Exit == -1)
	{
		Debug::D_Out << "[Main Menu] Texture Init Error" << std::endl;
	}
}

/*==============================================================================

	Controller Input Draw [Controller_Input.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef CONTROLLER_INPUT_H
#define CONTROLLER_INPUT_H
#include "Game_Window.h"
#include "Texture_Manager.h"
#include "Sprite.h"
#include "Sprite_Animation.h"
#include "KeyLogger.h"
#include "system_timer.h"

static float Alert_BG_X;
static float Alert_BG_y;
static float Alert_BG_Width;
static float Alert_BG_Height;

static float Alert_X;
static float Alert_y;
static float Alert_Width;
static float Alert_Height;

static float Button_X;
static float Button_y;
static float Button_Size;

enum class CONTROLLER_STATE
{
	NONE,
	INPUT,
	OUTPUT
};

void Controller_Set_Initialize();
void Controller_Set_Finalize();

void Controller_Set_Update();
void Controller_Set_Draw();

bool Controller_Set_UP();

void Controller_Texture();

#endif // CONTROLLER_INPUT_H
/*==============================================================================

	Fade In Out [Fade.h]

	Author : Choi HyungJoon

==============================================================================*/

#ifndef FADE_H
#define FADE_H
#include "Palette.h"
using namespace PALETTE;

#include <algorithm>
#include "Debug_ostream.h"
#include "direct3d.h"
#include "sprite.h"
#include "Texture_Manager.h"

enum class FADE_STATE
{
    NONE,
    FADE_IN,
    FADE_OUT,
    FINISHED_IN,
    FINISHED_OUT
};

void Fade_Initialize(void);
void Fade_Finalize(void);

void Fade_Update(double elapsed_time);
void Fade_Draw(void);

// True = Fade Out, False = Fade In
void Fade_Start(double duration, bool isFadeOut, Palette color = Black);

FADE_STATE Fade_GetState(void);
void Fade_SetState(FADE_STATE Fade);

#endif // FADE_H
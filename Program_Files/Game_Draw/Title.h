/*==============================================================================

	Game Title And Logo [Title.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef TITLE_H
#define TITLE_H
#include <d3d11.h>
#include <DirectXMath.h>

enum class INTRO_STATE
{                 
    FADE_IN_START,
    BAR_EXPAND,   
    NAME_REVEAL,  
    BAR_SHRINK,   
    WAIT_LOGO,    
    FADE_OUT_END, 
    DONE          
};                

void Title_Initialize();
void Title_Finalize();

void Title_Update(double elapsed_time);
void Title_Draw();

void Logo_Draw();

void Title_Texture();
#endif // TITLE_H
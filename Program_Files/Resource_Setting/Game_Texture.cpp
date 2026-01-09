/*==============================================================================

    Management Texture Resource [Game_Texture.h]

    Author : Choi HyungJoon

==============================================================================*/
#include "Game_Texture.h"
#include "Texture_Manager.h"
#include <unordered_map>
#include <string>

Texture_Manager* Texture_M;

void Game_Texture_Initialize()
{
    Texture_M = Texture_Manager::GetInstance();
    Game_Texture_Load();
}

void Game_Texture_Finalize()
{
}

void Game_Texture_Load()
{
    //------------------------------------------------Debug Texture------------------------------------------------//
    Texture_M->Load("Debug_Text", L"Resource/Texture/Other/Console_ASCII_512.png");
    
    Texture_M->Load("W", L"Resource/Texture/BG/RGBCMYK/White.png");
    Texture_M->Load("R", L"Resource/Texture/BG/RGBCMYK/Red.png");
    Texture_M->Load("G", L"Resource/Texture/BG/RGBCMYK/Green.png");
    Texture_M->Load("B", L"Resource/Texture/BG/RGBCMYK/Blue.png");
    Texture_M->Load("C", L"Resource/Texture/BG/RGBCMYK/Cyan.png");
    Texture_M->Load("M", L"Resource/Texture/BG/RGBCMYK/Magenta.png");
    Texture_M->Load("Y", L"Resource/Texture/BG/RGBCMYK/Yellow.png");
    Texture_M->Load("K", L"Resource/Texture/BG/RGBCMYK/Black.png");

    //-------------------------------------------<< In Game Texture >>-------------------------------------------//
    //-----------------------Mash Field Texture-----------------------//
    Texture_M->Load("Mash_Grass", L"Resource/Texture/Grass.jpg");
    Texture_M->Load("Mash_Dirt", L"Resource/Texture/Dirt.jpg");
    Texture_M->Load("Mash", L"Resource/Texture/OBJ/Mash.png");

    //----------------------------------------------<< BG Texture >>----------------------------------------------//

    //----------------------------------------------<< UI Texture >>----------------------------------------------//
    Texture_M->Load("Aim", L"Resource/Texture/Aim.png");
    Texture_M->Load("HP", L"Resource/Texture/HP.jpg");

    //---------------------UI : Main Menu Texture---------------------//
    Texture_M->Load("Start", L"Resource/Texture/UI/Menu_Start.png");
    Texture_M->Load("Settings", L"Resource/Texture/UI/Menu_Settings.png");
    Texture_M->Load("Exit", L"Resource/Texture/UI/Menu_Exit.png");

    //---------------------UI : Controller Texture---------------------//
    Texture_M->Load("UI_Controller_BG", L"Resource/Texture/Controller/Controller_Input_BG_Fixed.png");
    Texture_M->Load("UI_Controller_Input", L"Resource/Texture/Controller/Controller_Input_Alert_Fixed.png");
    Texture_M->Load("UI_Controller_Output", L"Resource/Texture/Controller/Controller_Output_Alert_Fixed.png");
    Texture_M->Load("UI_Controller_Button_Up", L"Resource/Texture/Controller/A_Pressed_UP.png");
    Texture_M->Load("UI_Controller_Button_Down", L"Resource/Texture/Controller/A_Pressed_Fixed.png");
    Texture_M->Load("UI_Controller_Enter_Up", L"Resource/Texture/Controller/Enter_Alpha.png");
    Texture_M->Load("UI_Controller_Enter_Down", L"Resource/Texture/Controller/Enter_Alpha_Press.png");


    //-------------------------------------------------Others Texture-------------------------------------------------//
    Texture_M->Load("Bismark_LOGO", L"Resource/Texture/Other/Bismark.jpg");
    Texture_M->Load("Bismark_LOGO_B", L"Resource/Texture/Other/Bismark_Black.jpg");
    Texture_M->Load("Bismark_Name", L"Resource/Texture/Other/Name_W.png");
    Texture_M->Load("Pixel_Withe", L"Resource/Texture/Other/W_Pixel.png");
    Texture_M->Load("Pixel_Black", L"Resource/Texture/Other/B_Pixel.png");

    // Sample
    Texture_M->Load("Effect_White", L"Resource/Texture/Explosion.png");
    Texture_M->Load("Cube_Sample", L"Resource/Texture/Cube_Sample.png");
    Texture_M->Load("Box_Sample", L"Resource/Texture/Box_Sample.png");

    Texture_M->Load("TextSample", L"Resource/Texture/w.png");
    Texture_M->Load("Target", L"Resource/Texture/Player_Avoid_Collision.png");

}
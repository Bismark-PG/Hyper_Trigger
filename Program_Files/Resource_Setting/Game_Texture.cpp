/*==============================================================================

    Management Texture Resource [Game_Texture.h]

    Author : Choi HyungJoon

==============================================================================*/
#include "Game_Texture.h"
#include "Texture_Manager.h"
#include <unordered_map>
#include <string>

static std::unordered_map<std::string, int> Texture_DB;

void Game_Texture_Initialize()
{
    Game_Texture_Load();
}

void Game_Texture_Finalize()
{
}

void Game_Texture_Load()
{
    // Sample
    Texture_M->Load("Debug_Text", L"Resource/Texture/Other/Console_ASCII_512.png");
    Texture_M->Load("Effect_White", L"Resource/Texture/Explosion.png");
    Texture_M->Load("Cube_Sample", L"Resource/Texture/Cube_Sample.png");
    Texture_M->Load("Box_Sample", L"Resource/Texture/Box_Sample.png");
    Texture_M->Load("Mash_Grass", L"Resource/Texture/Grass.jpg");
    Texture_M->Load("Mash_Dirt", L"Resource/Texture/Dirt.jpg");
    Texture_M->Load("TextSample", L"Resource/Texture/w.png");
    Texture_M->Load("Logo", L"Resource/Texture/Other/Bismark.jpg");

    // UI
    Texture_M->Load("Aim", L"Resource/Texture/Aim.png");
    Texture_M->Load("HP", L"Resource/Texture/HP.jpg");
}
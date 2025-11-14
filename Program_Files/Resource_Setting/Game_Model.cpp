/*==============================================================================

    Management Model Resource [Game_Model.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Game_Model.h"
#include "Texture_Manager.h"
#include "debug_ostream.h"  
#include "direct3d.h"       
#include <vector>
#include "WICTextureLoader11.h"

Game_Model* Game_Model::m_Instance = nullptr;
static std::unordered_map<std::string, MODEL*> Model_DB;

struct Vertex3D
{
    XMFLOAT3 position; // 頂点座標
    XMFLOAT3 normal;   //法線
    XMFLOAT4 color;    // 色
    XMFLOAT2 texcoord; // UV
};

Game_Model* Game_Model::GetInstance()
{
    if (!m_Instance)
    {
        m_Instance = new Game_Model();
    }
    return m_Instance;
}

void Game_Model::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    if (Model_M == nullptr)
    {
        Model_M = GetInstance();
        Model_M->Load(device, context);
    }
}

void Game_Model::Final()
{
    if (Model_M)
    {
        Model_M->Clear();
        delete Model_M;
        Model_M = nullptr;
    }
}

void Game_Model::Load(ID3D11Device* device, ID3D11DeviceContext* context)
{
    int g_TextureWhite = Texture_M->GetID("TextSample");

    MODEL* Player = ModelLoad("Resource/Model/Y_Bot.fbx", 0.01f, false);
    if (Player)
        Model_DB["player"] = Player;

    MODEL* sniperModel = ModelLoad("Resource/Model/ball.fbx", 0.5f, true);
    if (sniperModel)
        Model_DB["ball"] = sniperModel;

    MODEL* carModel = ModelLoad("Resource/Model/Benz_GLS_580_2020.fbx", 0.01f, true);
    if (carModel)
        Model_DB["car"] = carModel;
}

void Game_Model::Clear()
{
    Model_DB.clear();
}

MODEL* Game_Model::GetModel(const std::string& name)
{
    auto it = Model_DB.find(name);
    if (it != Model_DB.end())
    {
        return it->second;
    }

    Debug::D_Out << "Warning: Model not found in Game_Model: " << name.c_str() << std::endl;
    return nullptr;
}


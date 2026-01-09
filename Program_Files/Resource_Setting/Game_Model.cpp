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
using namespace DirectX;

Model_Manager* Model_Manager::GetInstance()
{
    static Model_Manager instance;
    return &instance;
}

void Model_Manager::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    Model_Manager::GetInstance()->Clear();
    Model_Manager::GetInstance()->Load(device, context);
}

void Model_Manager::Final()
{
    Model_Manager::GetInstance()->Clear();
}

void Model_Manager::Load(ID3D11Device* device, ID3D11DeviceContext* context)
{
    MODEL* Player = ModelLoad("Resource/Model/Y_Bot.fbx", 0.01f, false);
    if (Player)
        m_ModelDB["Player"] = Player;

    MODEL* carModel = ModelLoad("Resource/Model/Benz_GLS_580_2020.fbx", 0.01f, true);
    if (carModel)
        m_ModelDB["Car"] = carModel;

    MODEL* Sky = ModelLoad("Resource/Model/sky.fbx", 1.0f, false);
    if (Sky)
        m_ModelDB["Sky"] = Sky;

    MODEL* Ball = ModelLoad("Resource/Model/ball.fbx", 1.0f, false);
    if (Ball)
        m_ModelDB["Ball"] = Ball;
}

void Model_Manager::Clear()
{
    for (auto& pair : m_ModelDB)
    {
        ModelRelease(pair.second);
    }
    m_ModelDB.clear();
}

MODEL* Model_Manager::GetModel(const std::string& name)
{
    auto& DB = GetInstance()->m_ModelDB;

    auto it = DB.find(name);
    if (it != DB.end())
    {
        return it->second;
    }

    Debug::D_Out << "Warning: Model not found in Game_Model: " << name << std::endl;
    return nullptr;
}


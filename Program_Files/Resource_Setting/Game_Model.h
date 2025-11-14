/*==============================================================================

    Management Model Resource [Game_Model.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef GAME_MODEL_H
#define GAME_MODEL_H
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <string>
#include <unordered_map>
#include "model.h"
using namespace DirectX;

class Game_Model
{
private:
    static Game_Model* m_Instance;

    Game_Model() {}
    ~Game_Model() {}

    Game_Model(const Game_Model&) = delete;
    Game_Model& operator=(const Game_Model&) = delete;

    void Load(ID3D11Device* device, ID3D11DeviceContext* context);
    void Clear();

public:
    static Game_Model* GetInstance();

    static void Init(ID3D11Device* device, ID3D11DeviceContext* context);
    static void Final();

    static MODEL* GetModel(const std::string& name);
};

extern Game_Model* Model_M;

#endif // GAME_MODEL_H

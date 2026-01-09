/*==============================================================================

    Management Texture [Texture_Manager.cpp]

    Author : Choi HyungJoon

    - Note
        > Make With Singleton Pattern
        > Function Type  : Pointer

==============================================================================*/
#include "Texture_Manager.h"
#include "direct3d.h"
#include <windows.h>
#include "WICTextureLoader11.h"
using namespace DirectX;

Texture_Manager* Texture_Manager::GetInstance()
{
    static Texture_Manager Instance;
    return &Instance;
}

void Texture_Manager::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    // Unconditional Initialization
    m_device = device;
    m_context = context;
    Release_All(); // Clean start
}

void Texture_Manager::Final()
{
    Release_All();
    m_device = nullptr;
    m_context = nullptr;
}

void Texture_Manager::Release_All()
{
    m_textures.clear();
    m_TextureList.clear();
}

bool Texture_Manager::Load(const std::string& alias, const std::wstring& fileName)
{
    if (m_TextureList.count(alias))
        return false;

    Data Texture_Data;

    HRESULT hr = DirectX::CreateWICTextureFromFile(
        m_device,
        nullptr,
        fileName.c_str(),
        Texture_Data.Resource.GetAddressOf(),
        Texture_Data.SRV.GetAddressOf());

    if (FAILED(hr))
    {
        std::wstring Msg = L"Failed to load texture file: " + fileName;
        MessageBoxW(nullptr, Msg.c_str(), L"Texture Load Error", MB_OK | MB_ICONERROR);
        return false;
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> Tex2D;
    hr = Texture_Data.Resource.As(&Tex2D);
    if (SUCCEEDED(hr))
    {
        D3D11_TEXTURE2D_DESC desc;
        Tex2D->GetDesc(&desc);
        Texture_Data.Width = desc.Width;
        Texture_Data.Height = desc.Height;
    }

    m_textures.push_back(std::move(Texture_Data));
    int newId = static_cast<int>(m_textures.size() - 1);

    m_TextureList[alias] = newId;

    return true;
}

int Texture_Manager::GetID(const std::string& alias)
{
    auto it = m_TextureList.find(alias);

    if (it != m_TextureList.end())
        return it->second;

    return -1;
}

ID3D11ShaderResourceView* Texture_Manager::Get_Shader_Resource_View(int textureId)
{
    if (textureId < 0 || textureId >= m_textures.size())
        return nullptr;

    return m_textures[textureId].SRV.Get();
}

unsigned int Texture_Manager::Get_Width(int textureId)
{
    if (textureId < 0 || textureId >= m_textures.size())
        return 0;

    return m_textures[textureId].Width;
}

unsigned int Texture_Manager::Get_Height(int textureId)
{
    if (textureId < 0 || textureId >= m_textures.size())
        return 0;

    return m_textures[textureId].Height;
}
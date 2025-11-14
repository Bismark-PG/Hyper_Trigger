/*==============================================================================

	Management Texture [Texture_Manager.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef TEXTURE_H
#define TEXTURE_H

#include <d3d11.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <wrl/client.h> 

class Texture_Manager
{
public:

	static Texture_Manager* GetInstance();

	Texture_Manager(const Texture_Manager&) = delete;
	Texture_Manager& operator = (const Texture_Manager&) = delete;

	void Init(ID3D11Device* Device, ID3D11DeviceContext* Context);
	void Final();
	void Release_All();

	bool Load(const std::string& alias, const std::wstring& FileName);

	int GetID(const std::string& alias);

	ID3D11ShaderResourceView* Get_Shader_Resource_View(int Tex_ID);

	unsigned int Get_Width(int TexI_D);
	unsigned int Get_Height(int Tex_ID);

private:
	Texture_Manager() = default;
	~Texture_Manager() = default;

	struct Data
	{
		std::wstring FileName;
		Microsoft::WRL::ComPtr<ID3D11Resource> Resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV = nullptr;
		unsigned int Width = 0;
		unsigned int Height = 0;
	};

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;

	std::vector<Data> m_textures;
	std::unordered_map<std::string, int> m_TextureList;
};

extern Texture_Manager* Texture_M;

#endif // TEXTURE_H

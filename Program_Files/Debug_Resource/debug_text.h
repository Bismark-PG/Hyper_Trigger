/*==============================================================================

   Direct3D11用 デバックテキスト表示 [debug_text.h]
														 Author : Youhei Sato
														 Date   : 2025/06/15
												   Edited by : Choi HyungJoon
												 Edited Date : 2025/08/20
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef DEBUG_TEXT_H
#define DEBUG_TEXT_H

#include <d3d11.h>
#include <string>
#include <list>
#include <wrl/client.h> // Microsoft::WRL::ComPtrを使用する場合は必要
#include <DirectXMath.h>


namespace Text
{
	class DebugText
	{
	private:
		// Device, Context
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pContext = nullptr;

		// Text properties
		float m_OffsetX, m_OffsetY, m_Scale;
		UINT m_MaxLine{ 0 }; // For line limit
		UINT m_MaxCharactersPerLine{ 0 }; // For line wrap

		// Text data structures
		struct Characters
		{
			Characters(const DirectX::XMFLOAT4& color) : color(color) {}
			std::string characters;
			DirectX::XMFLOAT4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		};

		struct LineStrings
		{
			std::list<Characters> strings;
			UINT characterCount{ 0 }; // For line wrap and tab
			UINT spaceCount{ 0 };     // For character count
		};

		std::list<LineStrings> m_TextLines;
		UINT m_CharacterCount{ 0 };

		// Font texture (Manage in Texture Manager)
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
		UINT m_TextureWidth{ 0 };
		UINT m_TextureHeight{ 0 };

		// Buffers
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
		UINT m_BufferAllocatedCount{ 0 };

		// D3D States
		Microsoft::WRL::ComPtr<ID3D11BlendState> m_pBlendState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizerState;

		// Shaders
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVSConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerState;

	public:
		DebugText() = delete;
		DebugText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int fontTextureID, UINT screenWidth, UINT screenHeight);
		~DebugText() = default;

		void SetOffset(float x, float y)
		{
			m_OffsetX = x; m_OffsetY = y;
		}

		void SetScale(float scale)
		{
			m_Scale = scale;
		}

		void Print(const std::string& text, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });
		void Draw();
		void Clear();
		void Resize(UINT screenWidth, UINT screenHeight);

	private:
		struct Vertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT4 color;
			DirectX::XMFLOAT2 TexCoord;
		};
		void createBuffers(UINT characterCount);
	};
}
#endif // DEBUG_TEXT_H
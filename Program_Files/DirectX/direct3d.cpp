/*==============================================================================

   Reset Direct3D [direct3d.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include <d3d11.h>
#include "direct3d.h"
#include "debug_ostream.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

/* 各種インターフェース */
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pDeviceContext = nullptr;

static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11BlendState* g_pBlendStateMultiply = nullptr;
static ID3D11DepthStencilState* g_pDepthStencilStateDepthEnable = nullptr;
static ID3D11DepthStencilState* g_pDepthStencilStateDepthDisable = nullptr;
static ID3D11RasterizerState* g_pRasterizerState = nullptr;

/* バックバッファ関連 */
static ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
static ID3D11Texture2D* g_pDepthStencilBuffer = nullptr;
static ID3D11DepthStencilView* g_pDepthStencilView = nullptr;
static D3D11_TEXTURE2D_DESC g_BackBufferDesc{};
static D3D11_VIEWPORT g_Viewport{};

static bool configureBackBuffer(UINT width, UINT height); // バックバッファの設定・生成
static void releaseBackBuffer(); // バックバッファの解放

bool Direct3D_Initialize(HWND hWnd)
{
    /* デバイス、スワップチェーン、コンテキスト生成 */
    DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
    swap_chain_desc.Windowed = TRUE;
    swap_chain_desc.BufferCount = 2;
    // swap_chain_desc.BufferDesc.Width = 0;
    // swap_chain_desc.BufferDesc.Height = 0;
	// ⇒ ウィンドウサイズに合わせて自動的に設定される
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;

	// DISCARD or SEQUENTIAL>> Normal
	// Use FILP >> Skip Adaptive V-Sync
	// IF Want to Benchmark or Test Something Else etc...
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    //swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
    swap_chain_desc.OutputWindow = hWnd;

	/*
	IDXGIFactory1* pFactory;
	CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
	IDXGIAdapter1* pAdapter;
	pFactory->EnumAdapters1(1, &pAdapter); // セカンダリアダプタを取得
	pFactory->Release();
	DXGI_ADAPTER_DESC1 desc;
	pAdapter->GetDesc1(&desc); // アダプタの情報を取得して確認したい場合
	pAdapter->Release(); // D3D11CreateDeviceAndSwapChain()の第１引数に渡して利用し終わったら解放する
	*/

	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	swap_chain_desc.BufferDesc.Width = width;
	swap_chain_desc.BufferDesc.Height = height;

	UINT device_flags = 0;

#if defined(DEBUG) || defined(_DEBUG)
    device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };
    
    D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
 
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        device_flags,
        levels,
        ARRAYSIZE(levels),
        D3D11_SDK_VERSION,
        &swap_chain_desc,
        &g_pSwapChain,
        &g_pDevice,
        &feature_level,
        &g_pDeviceContext);

	if (FAILED(hr))
	{
#if defined(DEBUG) || defined(_DEBUG)
		MessageBox(hWnd, "Direct3Dの初期化に失敗しました", "エラー", MB_OK);
#else
		MessageBox(hWnd, L"Direct3Dの初期化に失敗しました", L"エラー", MB_OK);
#endif
		return false;
	}

	if (!configureBackBuffer(width, height))
	{
#if defined(DEBUG) || defined(_DEBUG)
		MessageBox(hWnd, "バックバッファの設定に失敗しました", "エラー", MB_OK);
#else
		MessageBox(hWnd, L"バックバッファの設定に失敗しました", L"エラー", MB_OK);
#endif
		return false;
	}

	// aブレンド(Blend)
	// RGBA A >> 好きに使っても良い
	// aデスト(Dest), aブレンド(Blend)

	// ブレンドステート設定
	D3D11_BLEND_DESC bd = {};
	bd.AlphaToCoverageEnable = FALSE;
	bd.IndependentBlendEnable = FALSE;
	bd.RenderTarget[0].BlendEnable = TRUE; // aブレンドするしない (Do Not Blend)

	// src >> ソース(今から描く絵 (色)) / dest >> すでに描かれた絵(色)

	// RGB
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // OP + >> Add Operand(演算子)
	// SrcRGB * SrcA + DestRGB * (1 - SrcA)

	// A
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;	  // ONE  (1)
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO; // ZERO (0)
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; // OP + >> Add Operand(演算子)
	// SrcA * 1 + DestA * 0

	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	g_pDevice->CreateBlendState(&bd, &g_pBlendStateMultiply);

	float blend_factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_pDeviceContext->OMSetBlendState(g_pBlendStateMultiply, blend_factor, 0xffffffff);

	D3D11_DEPTH_STENCIL_DESC dsd = {};
	dsd.StencilEnable = FALSE;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;

	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	g_pDevice->CreateDepthStencilState(&dsd, &g_pDepthStencilStateDepthEnable);

	dsd.DepthEnable = FALSE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	g_pDevice->CreateDepthStencilState(&dsd, &g_pDepthStencilStateDepthDisable);

	g_pDeviceContext->OMSetDepthStencilState(g_pDepthStencilStateDepthEnable, NULL);

	// ラスタライザステートの作成
	D3D11_RASTERIZER_DESC rd = {};
	rd.FillMode = D3D11_FILL_SOLID; // Draw Texture
	//rd.FillMode = D3D11_FILL_WIREFRAME; // Draw Line
	rd.CullMode = D3D11_CULL_BACK;
	//rd.CullMode = D3D11_CULL_NONE;
	rd.DepthClipEnable = TRUE;
	rd.MultisampleEnable = FALSE;
	g_pDevice->CreateRasterizerState(&rd, &g_pRasterizerState);

	// デバイスコンテキストにラスタライザーステートを設定
	g_pDeviceContext->RSSetState(g_pRasterizerState);

	return true;
}

void Direct3D_Finalize()
{
	releaseBackBuffer();

	SAFE_RELEASE(g_pDepthStencilStateDepthEnable);
	SAFE_RELEASE(g_pDepthStencilStateDepthDisable);
	SAFE_RELEASE(g_pBlendStateMultiply);
	SAFE_RELEASE(g_pRasterizerState);

	SAFE_RELEASE(g_pSwapChain);
	SAFE_RELEASE(g_pDeviceContext);
	SAFE_RELEASE(g_pDevice);
}

void Direct3D_Clear()
{
	//color change
	float clear_color[4] = { 0.2f, 0.4f, 0.2f, 1.0f };
	g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, clear_color);
	g_pDeviceContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	// レンダーターゲットビューとデプスステンシルビューの設定
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView,g_pDepthStencilView);
}

void Direct3D_Present()
{
	// スワップチェーンの表示
	// Present(1, 0) >> Present(0, 0)
	// Skip Adaptive V-Sync (IF Want to Benchmark or Test etc...)
	g_pSwapChain->Present(1, 0);
}

void Direct3D_Re_Size(UINT width, UINT height)
{
	if (!g_pSwapChain || width == 0 || height == 0)
		return;

	releaseBackBuffer();

	HRESULT hr = g_pSwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	if (FAILED(hr))
	{
		Debug::D_Out << "SwapChain ResizeBuffers failed." << std::endl;
		return;
	}

	if (!configureBackBuffer(width, height))
	{
		Debug::D_Out << "configureBackBuffer failed after resize." << std::endl;
	}
}

unsigned int Direct3D_GetBackBufferWidth()
{
	return g_BackBufferDesc.Width;
}

unsigned int Direct3D_GetBackBufferHeight()
{
	return g_BackBufferDesc.Height;
}

ID3D11Device* Direct3D_GetDevice()
{
	return g_pDevice;
}

ID3D11DeviceContext* Direct3D_GetContext()
{
	return g_pDeviceContext;
}

void Direct3D_SetDepthEnable(bool enable)
{
	if (enable)
		g_pDeviceContext->OMSetDepthStencilState(g_pDepthStencilStateDepthEnable, NULL);
	else
		g_pDeviceContext->OMSetDepthStencilState(g_pDepthStencilStateDepthDisable, NULL);
}

bool configureBackBuffer(UINT width, UINT height)
{
    HRESULT hr;

    ID3D11Texture2D* back_buffer_pointer = nullptr;

	// バックバッファの取得
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer_pointer);

    if (FAILED(hr)) {
		Debug::D_Out << "バックバッファの取得に失敗しました" << std::endl;
        return false;
    }

	// バックバッファのレンダーターゲットビューの生成
	hr = g_pDevice->CreateRenderTargetView(back_buffer_pointer, nullptr, &g_pRenderTargetView);

    if (FAILED(hr)) {
        back_buffer_pointer->Release();
		Debug::D_Out << "バックバッファのレンダーターゲットビューの生成に失敗しました" << std::endl;
        return false;
    }

	// バックバッファの状態（情報）を取得
    back_buffer_pointer->GetDesc(&g_BackBufferDesc);

	back_buffer_pointer->Release(); // バックバッファのポインタは不要なので解放

	// デプスステンシルバッファの生成
	D3D11_TEXTURE2D_DESC depth_stencil_desc{};
	depth_stencil_desc.Width = width;
	depth_stencil_desc.Height = height;
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_desc.SampleDesc.Count = 1;
	depth_stencil_desc.SampleDesc.Quality = 0;
	depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags = 0;
	hr = g_pDevice->CreateTexture2D(&depth_stencil_desc, nullptr, &g_pDepthStencilBuffer);

	if (FAILED(hr)) {
		Debug::D_Out << "デプスステンシルバッファの生成に失敗しました" << std::endl;
		return false;
	}

	// デプスステンシルビューの生成
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	depth_stencil_view_desc.Format = depth_stencil_desc.Format;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	depth_stencil_view_desc.Flags = 0;
	hr = g_pDevice->CreateDepthStencilView(g_pDepthStencilBuffer, &depth_stencil_view_desc, &g_pDepthStencilView);

	if (FAILED(hr)) {
		Debug::D_Out << "デプスステンシルビューの生成に失敗しました" << std::endl;
		return false;
	}

	// ビューポートの設定
	g_Viewport.TopLeftX = 0.0f;
	g_Viewport.TopLeftY = 0.0f;
	g_Viewport.Width = static_cast<FLOAT>(width);
	g_Viewport.Height = static_cast<FLOAT>(height);
	g_Viewport.MinDepth = 0.0f;
	g_Viewport.MaxDepth = 1.0f;
	g_pDeviceContext->RSSetViewports(1, &g_Viewport); // ビューポートの設定

    return true;
}

void releaseBackBuffer()
{
	if (g_pDeviceContext)
		g_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	SAFE_RELEASE(g_pRenderTargetView);
	SAFE_RELEASE(g_pDepthStencilBuffer);
	SAFE_RELEASE(g_pDepthStencilView);
}
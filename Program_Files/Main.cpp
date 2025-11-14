/*==============================================================================

	Project Main CPP [Update_Game.cpp]

	Project : DirectX 3D Sample

	Author : Choi HyungJoon
	Date : 2025. 00. 00
	Version : 1.0.0

==============================================================================*/
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN
#include "System_Logic_Manager.h"
#include "Game_Header_Manager.h"
using namespace PALETTE;

Shader_Manager*  Shader_M  = nullptr;
Texture_Manager* Texture_M = nullptr;
Game_Model* Model_M = nullptr;
Window_Manager* Window_M = nullptr;
Audio_Manager* Sound_M = nullptr;

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR IpCmdline, _In_ int nCmdShow)
{
	Window_M = Window_Manager::GetInstance();
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	HWND hWnd = Window_M->Init(hInstance, nCmdShow);
	if (!hWnd)
		return -1;

	// Initialize Direct 3D
	Direct3D_Initialize(hWnd);
	ID3D11Device* Device = Direct3D_GetDevice();
	ID3D11DeviceContext* Context = Direct3D_GetContext();

	// Initialize System Logic
	System_Initialize(hWnd, Device, Context);

	std::unique_ptr<Text::DebugText> Debug;

	auto Create_DebugText = [&]()
		{
			Debug = std::make_unique<Text::DebugText>(
				Device, Context,
				Texture_M->GetID("Debug_Text"),
				Direct3D_GetBackBufferWidth(), Direct3D_GetBackBufferHeight());
			Debug->SetScale(0.75f);
			Debug->SetOffset(10.0f, 10.0f);
		};

	Create_DebugText();

	// Show Mouse (True = Show // False = Don`t Show)
	Mouse_SetVisible(false);
	Mouse_SetMode(MOUSE_POSITION_MODE_RELATIVE);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Set Frame
	double Exec_Last_Time = SystemTimer_GetTime();
	double Fps_Last_Time = Exec_Last_Time;
	double Current_Time = 0.0;
	ULONG Frame_Count = 0;
	double FPS = 0.0; // Frame Per Second

	// Make Game Loop & Message Loop
	MSG Message = {};

	if (FAILED(hr))
	{
		MessageBoxW(nullptr, L"Failed To Set Function In Main.cpp : [CoInitializeEx]", L">> FATAL ERROR <<", MB_OK | MB_ICONERROR);
		return (int)Message.wParam;;
	}

	do
	{
		if (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		
		// Set Game
		else
		{
			// Time Set
			Current_Time = SystemTimer_GetTime(); // Get System Time
			double Elapsed_Time = Current_Time - Fps_Last_Time; // Get Time For FPS

			//  Set FPS
			if (Elapsed_Time >= 1.0)
			{
				FPS = Frame_Count / Elapsed_Time;
				Fps_Last_Time = Current_Time; // Save FPS
				Frame_Count = 0; // Clear Count
			}

			// Set Elapsed Time
			Elapsed_Time = Current_Time - Exec_Last_Time;
			// If you want to limit FPS, set condition here
			if (Elapsed_Time >= (1.0 / 60.0)) // 60 FPS
			{
				Exec_Last_Time = Current_Time; // Save Last Time
				
				// Set Key Logger With FPS
				KeyLogger_Update();

				// Update Game Texture
				Game_Update(Elapsed_Time);
				SpriteAni_Update(Elapsed_Time);

				// Draw Texture
				Direct3D_Clear();
				Sprite_Begin();

				Game_Draw();

				// Show FPS
#if defined(DEBUG) || defined(_DEBUG)
				std::stringstream Debug_State;
				Debug_State << "FPS : " << FPS << "\n" << std::endl;

				Debug_State << "Player POS : X = " << Player_Get_POS().x << " Y = "
					<< Player_Get_POS().y << " Z = " << Player_Get_POS().z << std::endl;
				Debug_State << "P_Camera POS : X = " << Player_Camera_Get_POS().x << " Y = "
					<< Player_Camera_Get_POS().y << " Z = " << Player_Camera_Get_POS().z << "\n" << std::endl;

				Debug_State << "D_Camera POS : X = " << Debug_Camera_Get_POS().x << " Y = "
					<< Debug_Camera_Get_POS().y << " Z = " << Debug_Camera_Get_POS().z << std::endl;
				Debug_State << "D_Camera FOV : " << Debug_Camera_Get_FOV() << "\n" << std::endl;

				Debug_State << "Front	 : X = " << Debug_Camera_Get_Front().x << " Y = "
					<< Debug_Camera_Get_Front().y << " Z = " << Debug_Camera_Get_Front().z << std::endl;
				Debug_State << "Vertical : X = " << Debug_Camera_Get_Vertical().x << " Y = "
					<< Debug_Camera_Get_Vertical().y << " Z = " << Debug_Camera_Get_Vertical().z << std::endl;
				Debug_State << "Horizon	 : X = " << Debug_Camera_Get_Horizon().x << " Y = "
					<< Debug_Camera_Get_Horizon().y	<< " Z = " << Debug_Camera_Get_Horizon().z << std::endl;

				Debug_State << "Sights : " << static_cast<int>(Player_Camera_Get_Now_Sights()) << std::endl;
				Debug->Print(Debug_State.str(), Light_Green);

				Debug->Draw();
				Debug->Clear();
#endif	

				Direct3D_Present();

				Frame_Count++;
			}	
		}
	} while (Message.message != WM_QUIT);
	
	System_Finalize();

	return (int)Message.wParam;
}
/*==============================================================================

	Make Window [Game_Window.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#define WIN32_LEAN_AND_MEAN
#include <sdkddkver.h>
#include <Windows.h>

class Window_Manager
{
public:
	static Window_Manager* GetInstance();

	// Singleton Safety
	Window_Manager(const Window_Manager&) = delete;
	Window_Manager& operator=(const Window_Manager&) = delete;

	HWND Init(HINSTANCE hInstance, int nCmdShow);
	void Final();

	HWND GetHWND() const { return m_hWnd; }

	UINT GetWidth() const { return m_ScreenWidth; }
	UINT GetHeight() const { return m_ScreenHeight; }

	void HandleResize(UINT width, UINT height);

	bool IsMessageBoxActive() const { return m_IsMessageBoxOpen; }
	void SetMessageBoxActive(bool active) { m_IsMessageBoxOpen = active; }

private:
	Window_Manager() = default;
	~Window_Manager() = default;

	HWND m_hWnd = nullptr;
	HINSTANCE m_hInstance = nullptr;
	UINT m_ScreenWidth = 1920;
	UINT m_ScreenHeight = 1080;
	bool m_IsMessageBoxOpen = false;

#if defined(DEBUG) || defined(_DEBUG)
	static constexpr char WINDOW_CLASS[] = "Hyper Trigger";
	static constexpr char TITLE[] = "Hyper Trigger";
#else
	static constexpr wchar_t WINDOW_CLASS[] = L"Hyper Trigger";
	static constexpr wchar_t TITLE[] = L"Hyper Trigger";
#endif
};

extern float SCREEN_WIDTH;
extern float SCREEN_HEIGHT;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


#endif // GAME_WINDOW_H
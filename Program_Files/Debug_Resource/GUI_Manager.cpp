/*==============================================================================

	Manage GUI Resource [GUI_Manager.h]

	Author : Choi HyungJoon

==============================================================================*/
#include "GUI_Manager.h"

void GUI_Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // keyboard control
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // docking..?

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, context);
}

void GUI_Final()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

/*==============================================================================

	Manage System Logic [System_Logic_Manager.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef SYSTEM_LOGIC_MANAGER
#define SYSTEM_LOGIC_MANAGER
#include "Game_Header_Manager.h"

class System_Manager
{
public:
    static System_Manager& GetInstance()
    {
        static System_Manager instance;
        return instance;
    }

    void Initialize(HWND hWnd, ID3D11Device* Device, ID3D11DeviceContext* Context);
    void Finalize();

    System_Manager(const System_Manager&) = delete;
    System_Manager& operator=(const System_Manager&) = delete;

private:
    System_Manager() = default;
    ~System_Manager() = default;

};

#endif // SYSTEM_LOGIC_MANAGER
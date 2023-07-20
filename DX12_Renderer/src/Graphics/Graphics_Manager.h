#pragma once

#include "../extern/d3dx12.h"
#include <wrl.h>
#include <windows.h>

using Microsoft::WRL::ComPtr;

class Graphics_Manager
{
public:
    static Graphics_Manager& get_instance() { static Graphics_Manager instance; return instance; }
    
    Graphics_Manager(const Graphics_Manager&) = delete;
    Graphics_Manager& operator=(const Graphics_Manager&) = delete;
    ~Graphics_Manager();

    void init(HWND* const window_handle, HINSTANCE hInstance);
    void update();
    void render();
    void close();
    
private:
    HWND* m_window_handle;
    Graphics_Manager();
};

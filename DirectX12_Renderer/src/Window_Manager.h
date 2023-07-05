#pragma once
#include <windows.h>

class Window_Manager
{
public:
    Window_Manager(HINSTANCE hInstance);
    ~Window_Manager() = default;
    bool create_window();
private:
    bool create_window_class();
    bool create_window_handler();
    
private:
    HINSTANCE m_hInstance;
    WNDCLASSEX m_window_class;
    HWND m_window_handler;
    
};

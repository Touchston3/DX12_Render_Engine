#include "Window_Manager.h"

Window_Manager::Window_Manager(HINSTANCE hInstance) : m_hInstance(hInstance)
{
    
}

bool Window_Manager::create_window()
{
    if(!create_window_class())
        return false;
    if(!create_window_handler())
        return false;
    ShowWindow(m_window_handler, SW_SHOW);

    //Create Event Loop
    MSG msg = { 0 };
    while( GetMessage(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    

    return true;
}

bool Window_Manager::create_window_class()
{
    m_window_class = { 0 };
    m_window_class.cbSize = sizeof(m_window_class);
    m_window_class.style = CS_OWNDC;
    m_window_class.lpfnWndProc = window_event_handler;
    m_window_class.cbClsExtra = 0;
    m_window_class.cbWndExtra = 0;
    m_window_class.hInstance = m_hInstance;
    m_window_class.hIcon = nullptr;
    m_window_class.hCursor = nullptr;
    m_window_class.hbrBackground = nullptr;
    m_window_class.lpszMenuName = nullptr;
    m_window_class.lpszClassName = L"Main_Window";
    m_window_class.hIconSm = nullptr;

    //Register Window Class
    return RegisterClassEx( &m_window_class ) != 0; //Has weird return types. Returns 0 on failure tho
}

bool Window_Manager::create_window_handler()
{
    m_window_handler = CreateWindowEx(
        0, L"Main_Window", L"Renderer",
        WS_OVERLAPPEDWINDOW ,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, m_hInstance, nullptr
    );
    return &m_window_handler != nullptr;
}

LRESULT Window_Manager::window_event_handler(HWND window_handler, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch( msg )
    {
    case WM_DESTROY:
        handle_destroy();
        break;
        
    }
    return DefWindowProc(window_handler, msg, wparam, lparam);
}

void Window_Manager::handle_destroy()
{
    PostQuitMessage( 0 );
}

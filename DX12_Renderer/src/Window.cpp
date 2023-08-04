#include "Window.h"

#include <windowsx.h>

#include "Input_Manager.h"
#include <WinUser.h>
#include "Log/Log_Manager.h"

using namespace void_renderer;

Window::Win32_Window_Class::Win32_Window_Class(std::wstring class_name, HINSTANCE hInstance) :
    m_hInstance(hInstance),
    m_class_name(class_name),
    m_class_definition
    ({
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_OWNDC,
        .lpfnWndProc = DefWindowProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = hInstance,
        .hIcon = nullptr,
        .hCursor = nullptr,
        .hbrBackground = nullptr,
        .lpszMenuName = nullptr,
        .lpszClassName = class_name.c_str(),
        .hIconSm = nullptr,
    })
{
    if (RegisterClassExW(&m_class_definition) == 0)
    {
        DEBUG_LOG_FATAL(L"Failed to register windows class {}", "");
    }
}

Window::Win32_Window_Class::~Win32_Window_Class()
{
    UnregisterClass(m_class_name.c_str(), m_hInstance);
}

LRESULT Window::Win32_Window_Class::msg_handler(HWND window_handler, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_MOUSEMOVE:
    {
        Mouse_Move_Event e(nullptr, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
        m_message_handler(e);
        break;
    }
    case WM_LBUTTONDOWN:
        break;
    case WM_RBUTTONDOWN:
        break;
    case WM_CHAR:
    {
        Key_Event e(nullptr, 'X', Key_State::VOID_KEY_PRESSED);
        m_message_handler(e);
        break;
    }
    case WM_CLOSE:
    {
        DestroyWindow(window_handler);
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    default:
        break;
    }
    return DefWindowProc(window_handler, msg, wparam, lparam);
}


Window::Window(int width, int height, HINSTANCE hInstance, std::wstring class_name) :
    m_hInstance(hInstance),
    m_width(width),
    m_height(height),
    m_class_name(class_name),
    m_window_class(Win32_Window_Class(class_name, hInstance)),
    m_msg({}),
    m_window_handler(CreateWindow
    (
        class_name.c_str(),
        L"Void Engine",
        WS_OVERLAPPEDWINDOW,
        100, 100,
        width, height,
        nullptr,
        nullptr,
        hInstance,
        nullptr)
    ),
    m_renderer(nullptr)
{
    if (m_window_handler == nullptr)
    {
        DEBUG_LOG_FATAL(L"Failed to create window {}", "");
    }
    ShowWindow(m_window_handler, SW_SHOW);
    this->m_renderer = new Renderer(this);
}

Window::Window(int width, int height, HINSTANCE hInstance) : Window(width, height, hInstance, L"MAIN_WINDOW") {}

Window::~Window()
{
    delete(m_renderer);
}

bool Window::is_closed()
{
    return GetMessage(&m_msg, nullptr, 0, 0) <= 0;    
}

void Window::handle_events()
{
    TranslateMessage(&m_msg);
    DispatchMessage(&m_msg);
}

void Window::attach_event_handler(std::function<void(Event&)> handler_cb)
{
    Win32_Window_Class::set_message_handler(handler_cb);
    SetWindowLongPtr(m_window_handler, GWLP_WNDPROC, (LONG_PTR)m_window_class.msg_handler);
}

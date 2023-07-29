#include "Window.h"

#include <windowsx.h>

#include "Input_Manager.h"
#include <WinUser.h>

#include "../../../../../../../../Program Files (x86)/Windows Kits/10/Include/10.0.22000.0/ucrt/assert.h"

using namespace void_renderer;

Window::Win32_Window_Class::Win32_Window_Class(const wchar_t* class_name, HINSTANCE hInstance) :
    m_hInstance(hInstance),
    m_class_definition({}),
    m_class_name(class_name)
{
    m_class_definition.cbSize = sizeof(WNDCLASSEX);
    m_class_definition.style = CS_HREDRAW | CS_VREDRAW;
    m_class_definition.lpfnWndProc = DefWindowProc;
    m_class_definition.cbClsExtra = 0;
    m_class_definition.cbWndExtra = 0;
    m_class_definition.hInstance = m_hInstance;
    m_class_definition.hIcon = nullptr;
    m_class_definition.hCursor = nullptr;
    m_class_definition.hbrBackground = nullptr;
    m_class_definition.lpszMenuName = nullptr;
    m_class_definition.lpszClassName = class_name;
    m_class_definition.hIconSm = nullptr;

    assert(RegisterClassEx(&m_class_definition) != 0);
}

Window::Win32_Window_Class::~Win32_Window_Class()
{
    UnregisterClass(m_class_name, m_hInstance);
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
    case WM_DESTROY:
        break;
    default:
        break;
    }
    return DefWindowProc(window_handler, msg, wparam, lparam);
}


Window::Window(HINSTANCE hInstance) :
    m_hInstance(hInstance),
    m_window_class(Win32_Window_Class(CLASSNAME, m_hInstance)),
    m_window_handler(CreateWindowExW(
        0, CLASSNAME, L"Renderer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
        nullptr, nullptr, m_hInstance, nullptr
    ))
{
    assert(m_window_handler != nullptr);
    ShowWindow(m_window_handler, SW_SHOW);
}

Window::Window(HINSTANCE hInstance, Win32_Window_Class window_class) :
    m_hInstance(hInstance),
    m_window_class(window_class),
    m_window_handler(CreateWindowEx(
        0, CLASSNAME, L"Renderer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
        nullptr, nullptr, m_hInstance, nullptr
    ))
{
    assert(m_window_handler != nullptr);
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

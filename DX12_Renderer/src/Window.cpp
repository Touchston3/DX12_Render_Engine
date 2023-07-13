#include "Window.h"
#include "Input_Manager.h"
#include <WinUser.h>


Window::Win32_Window_Class::Win32_Window_Class(const char* class_name, HINSTANCE hInstance) :
    m_hInstance(hInstance),
    m_class_definition({ 0 }),
    m_class_name(class_name),
    m_message_handler(nullptr)
{
    m_class_definition.cbSize = sizeof(m_window_class);
    m_class_definition.style = CS_OWNDC;
    m_class_definition.lpfnWndProc = DefWindowProc;
    m_class_definition.cbClsExtra = 0;
    m_class_definition.cbWndExtra = 0;
    m_class_definition.hInstance = m_hInstance;
    m_class_definition.hIcon = nullptr;
    m_class_definition.hCursor = nullptr;
    m_class_definition.hbrBackground = nullptr;
    m_class_definition.lpszMenuName = nullptr;
    m_class_definition.lpszClassName =  class_name;
    m_class_definition.hIconSm = nullptr;

    RegisterClassEx(&m_class_definition);
}

Window::Win32_Window_Class::~Win32_Window_Class()
{
    UnregisterClass(m_class_name, m_hInstance);
}

LRESULT Window::Win32_Window_Class::msg_handler(HWND window_handler, UINT msg, WPARAM wparam, LPARAM lparam)
{
}


Window::Window(HINSTANCE hInstance) :
    m_hInstance(hInstance),
    m_window_class(Win32_Window_Class(CLASSNAME, m_hInstance))
{
}

Window::Window(HINSTANCE hInstance, Win32_Window_Class window_class) : 
    m_hInstance(hInstance),
    m_window_class(window_class)
{
}
void Window::attach_event_handler(std::function<void(Event&)> handler_cb)
{
    m_window_class.set_message_handler(handler_cb);
    SetWindowLongPtr(m_window_handler, GWLP_WNDPROC, (LONG_PTR)m_window_class.msg_handler);
}

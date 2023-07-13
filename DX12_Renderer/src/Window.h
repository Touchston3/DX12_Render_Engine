#pragma once
#include <windows.h>
#include <functional>
#include "Event.h"

class Window
{
    class Win32_Window_Class
    {
    public:
        Win32_Window_Class(const char* class_name, HINSTANCE hInstance);
        ~Win32_Window_Class();

        static LRESULT CALLBACK msg_handler(HWND window_handler, UINT msg, WPARAM wparam, LPARAM lparam);
        void set_message_handler(std::function<void(Event&)> message_handler) { m_message_handler = message_handler; }
        std::function<void(Event&)> get_message_handler() const { return m_message_handler; }
    private:
        HINSTANCE m_hInstance;
        WNDCLASSEX m_class_definition;
        const char* m_class_name;
        std::function<void(Event&)> m_message_handler;
    };

public:
    void attach_event_handler(std::function<void(Event&)> handler_cb);
    Window(HINSTANCE hInstance);
    Window(HINSTANCE hInstance, Win32_Window_Class window_class);

private:
    const char* CLASSNAME = "Main_Window";
    HINSTANCE m_hInstance;
    Win32_Window_Class m_window_class;
    HWND m_window_handler;
};

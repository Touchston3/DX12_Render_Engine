#pragma once
#include <windows.h>
#include <functional>
#include "Event.h"
namespace void_renderer
{
    class Window
    {
        class Win32_Window_Class
        {
        public:
            Win32_Window_Class(const wchar_t* class_name, HINSTANCE hInstance);
            ~Win32_Window_Class();

            static LRESULT CALLBACK msg_handler(HWND window_handler, UINT msg, WPARAM wparam, LPARAM lparam);
            static void set_message_handler(const std::function<void(Event&)>& message_handler) { m_message_handler = message_handler; }
            static std::function<void(Event&)> get_message_handler() { return m_message_handler; }
        private:
            HINSTANCE m_hInstance;
            WNDCLASSEX m_class_definition;
            const wchar_t* m_class_name;
            inline static std::function<void(Event&)> m_message_handler = nullptr;
        };

    public:
        void attach_event_handler(std::function<void(Event&)> handler_cb);
        Window(HINSTANCE hInstance);
        Window(HINSTANCE hInstance, Win32_Window_Class window_class);
        void tmp_window_loop();
        HWND get_window_handler() const { return m_window_handler; }

        bool is_closed();
        void handle_events();
    private:
        const wchar_t* CLASSNAME = L"Main_Window";
        HINSTANCE m_hInstance;
        Win32_Window_Class m_window_class;
        HWND m_window_handler;
        MSG m_msg;
    };
}
#pragma once
#include <windows.h>
#include <functional>
#include "Event.h"
#include "string"
#include "Graphics/Renderer.h"
namespace void_renderer
{
    class Window
    {
        class Win32_Window_Class
        {
        public:
            Win32_Window_Class(std::wstring class_name, HINSTANCE hInstance);
            ~Win32_Window_Class();

            static LRESULT CALLBACK msg_handler(HWND window_handler, UINT msg, WPARAM wparam, LPARAM lparam);
            static void set_message_handler(const std::function<void(Event&)>& message_handler) { m_message_handler = message_handler; }
            static std::function<void(Event&)> get_message_handler() { return m_message_handler; }

            std::wstring get_class_name() const { return m_class_name; }

        private:
            HINSTANCE m_hInstance;
            WNDCLASSEX m_class_definition;
            std::wstring m_class_name;
            inline static std::function<void(Event&)> m_message_handler = nullptr;
        };

    public:
        void attach_event_handler(std::function<void(Event&)> handler_cb);
        Window(int width, int height, HINSTANCE hInstance);
        Window(int width, int height, HINSTANCE hInstance, std::wstring class_name);
        ~Window();

        HWND get_window_handler() const { return m_window_handler; }
        int get_width() const { return m_width; }
        int get_height() const { return m_height; }
        void render() const { m_renderer->render(); }

        bool is_closed();
        void handle_events();
    private:
        HINSTANCE m_hInstance;
        int m_width;
        int m_height;
        std::wstring m_class_name;

        Win32_Window_Class m_window_class;
        MSG m_msg;
        HWND m_window_handler;

        Renderer* m_renderer;
    };
}
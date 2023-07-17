#pragma once
#include <windows.h>


class Window_Manager
{
public:
    Window_Manager(HINSTANCE hInstance);
    ~Window_Manager();
    bool create_window();
    HWND get_window_handler() const { return m_window_handler; }
    HINSTANCE get_hInstance() const { return m_hInstance; }

private:
    bool create_window_class();
    bool create_window_handler();

    //Message Handling
    static LRESULT CALLBACK window_event_handler(HWND window_handler, UINT msg, WPARAM wparam, LPARAM lparam); //Windows callback for message handling
    static void handle_destroy();
    static void handle_mouse_move(WPARAM wparam, LPARAM lparam);
    static void handle_keystroke_input(WPARAM wparam, LPARAM lparam);
    static void handle_left_click(WPARAM wparam, LPARAM lparam);
    static void handle_right_click(WPARAM wparam, LPARAM lparam);
private:
    HINSTANCE m_hInstance;
    WNDCLASSEX m_window_class;
    HWND m_window_handler;
    char* m_window_classname;
    char* m_windowname;

};


/*
class Window
{
    class Win32_Window_Class 
    {
        m_class_definition;
        m_class_name;
        register_class();
        unregister_class();
        static default_msg_handler()
        static actual_msg_handler() { m_cb(Event e()); }
    }

    public:
    void attach_event_handler(std::function<void(Event&)> handler_cb)
    
    private:
    m_hInstance;
    m_window_class;
    m_window_object;
    m_message_handler;
}
*/
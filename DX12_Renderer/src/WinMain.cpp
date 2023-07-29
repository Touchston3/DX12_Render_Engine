#include <d3d12.h>
#include "Window.h"
#include "Log/Log_Manager.h"
#include "Input_Manager.h"
#include "Graphics/Graphics_Manager.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PSTR lpCmdLine,
    int nCmdShow)
{
    
    void_renderer::Log_Manager::get()->add_log_target(Log_Target::Console, L"CONSOLE");
    void_renderer::Log_Manager::get()->add_log_target(Log_Target::Debugger, L"DEBUG");
    void_renderer::Log_Manager::get()->set_application_log_level(Log_Level::Info);
    
    void_renderer::Window window(hInstance);
    window.attach_event_handler(void_renderer::Input_Manager::handle_window_input); //I feel like I shouldn't need this to be a static method.
    
    Graphics_Manager::get()->init(window.get_window_handler());
    while( !window.is_closed() )
    {
        Graphics_Manager::get()->update();
        Graphics_Manager::get()->populate_command_list();
        window.handle_events();
    }
    
    return 0;
}
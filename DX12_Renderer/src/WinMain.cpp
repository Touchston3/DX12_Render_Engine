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
    
    void_renderer::Log_Manager::get_instance().add_log_target(Log_Target::Console, L"CONSOLE");
    void_renderer::Log_Manager::get_instance().set_application_log_level(Log_Level::Info);
    
    void_renderer::Window window(hInstance);
    window.attach_event_handler(void_renderer::Input_Manager::handle_window_input); //I feel like I shouldn't need this to be a static method.
    
    Graphics_Manager::get_instance().init(window.get_window_handler());
    while( !window.is_closed() )
    {
        window.handle_events();
        Graphics_Manager::get_instance().update();
    }
    //window.tmp_window_loop();
    
    return 0;
}
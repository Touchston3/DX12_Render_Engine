#include "Window.h"
#include "Logger.h"
#include "Input_Manager.h"
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PSTR lpCmdLine,
    int nCmdShow)
{
    void_renderer::Window window(hInstance);
    //Create Window Class
    Logger::get_instance()->create_console();
    
    window.attach_event_handler((std::function<void(void_renderer::Event&)>)void_renderer::Input_Manager::get_instance().handle_window_input);
    window.tmp_window_loop();
    return 0;
}
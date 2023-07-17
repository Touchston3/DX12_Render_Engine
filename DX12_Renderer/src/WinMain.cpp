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
    void_renderer::Logger::get_instance()->create_console();
    
    window.attach_event_handler(void_renderer::Input_Manager::handle_window_input); //I feel like I shouldn't need this to be a static method. 
    window.tmp_window_loop();
    return 0;
}
#include "Window_Manager.h"
#include "Logger.h"
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PSTR lpCmdLine,
    int nCmdShow)
{
    //Create Window Class
    Logger::get_instance()->create_console();
    Window_Manager wm(hInstance);
    
    wm.create_window();
    Logger::get_instance()->log(1, LOG, "Test Log");
    return 0;
}
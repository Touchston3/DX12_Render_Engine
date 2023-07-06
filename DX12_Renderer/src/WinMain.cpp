#include "Window_Manager.h"
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PSTR lpCmdLine,
    int nCmdShow)
{
    //Create Window Class
    Window_Manager wm(hInstance);
    
    wm.create_window();
    
    return 0;
}
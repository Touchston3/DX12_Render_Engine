#include "Input_Manager.h"
#include "Logger.h"
using namespace void_renderer;
void Input_Manager::handle_window_input(Event& e)
{
    switch(e.m_event_type)
    {
    case Event_Type::VOID_MOUSE_MOVE:
        Logger::get_instance()->log(__LINE__, LOG, "Mouse Move");
        break;
    case Event_Type::VOID_KEY_EVENT:
        OutputDebugStringW(L"TESTING"); //This works. I am still not sure how I want to do debugging
        break;
    case Event_Type::VOID_MOUSE_CLICK:
        break;
    }
}

Input_Manager::~Input_Manager()
{
    
}

Input_Manager::Input_Manager()
{
}

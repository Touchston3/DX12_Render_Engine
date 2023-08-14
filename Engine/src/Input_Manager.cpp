#include "Input_Manager.h"
#include "Log/Log_Manager.h"
using namespace VE;
void Input_Manager::handle_window_input(Event& e)
{
    switch(e.m_event_type)
    {
    case Event_Type::VOID_MOUSE_MOVE:
        {
            break;
        }
    case Event_Type::VOID_KEY_EVENT:
        {
            break;
        }
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

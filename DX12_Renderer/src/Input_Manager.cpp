#include "Input_Manager.h"
#include "Log/Log_Manager.h"
using namespace void_renderer;
void Input_Manager::handle_window_input(Event& e)
{
    switch(e.m_event_type)
    {
    case Event_Type::VOID_MOUSE_MOVE:
        {
            Log_Info l = { __LINE__, L"test", Log_Level::Info, L"TESTING"};
            Log_Manager::get_instance()->log(l);
            break;
        }
    case Event_Type::VOID_KEY_EVENT:
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

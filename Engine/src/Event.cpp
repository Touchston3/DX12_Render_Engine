#include "Event.h"
using namespace VE;
Event::Event(void* const source, Event_Type event_type) :
    m_source(source),
    m_event_type(event_type)
{
}

Mouse_Move_Event::Mouse_Move_Event(void* const source, int x, int y) :
    Event(source, Event_Type::VOID_MOUSE_MOVE),
    m_x(x),
    m_y(y)
{
}

Mouse_Click_Event::Mouse_Click_Event(void* const source, int x, int y, Mouse_State click_state, Mouse_Button button) :
    Event(source, Event_Type::VOID_MOUSE_CLICK),
    m_x(x),
    m_y(y),
    m_click_state(click_state),
    m_button(button)
{
}

Key_Event::Key_Event(void* const source, char key, Key_State key_press_state) :
    Event(source, Event_Type::VOID_KEY_EVENT),
    m_key(key),
    m_key_press_state(key_press_state)
{
}

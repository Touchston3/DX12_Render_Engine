#pragma once
enum Event_Type
{
    MOUSE_MOVE,
    MOUSE_CLICK,
    KEY_EVENT
};
enum Mouse_State
{
    PRESSED,
    RELEASED,
    DOUBLE_CLICK
};
enum Mouse_Button
{
    LEFT,
    MIDDLE,
    RIGHT
};
enum Key_State
{
    PRESSED,
    RELEASED,
};
class Event
{
public:
    Event(void* const source, Event_Type type) : m_source(source), m_type(type) {}
    void* m_source;
    Event_Type m_type;
    
};
class Mouse_Move_Event : Event 
{
public: 
    Mouse_Move_Event(void* const source, int x, int y) : Event(source, MOUSE_MOVE), m_x(x), m_y(y) {}
    int m_x, m_y;
};

class Mouse_Click_Event : Event
{
public:
    Mouse_Click_Event(void* const source, int x, int y, Mouse_State click_state, Mouse_Button button) : Event(source, MOUSE_CLICK), m_x(x), m_y(y), m_click_state(click_state), m_button(button) {}
    int m_x, m_y;
    Mouse_State m_click_state;
    Mouse_Button m_button;
};

class Key_Event : Event
{
public:
    Key_Event(void* const source, char key, Key_State key_press_state) : Event(source, KEY_EVENT), m_key(key), m_key_press_state(key_press_state) {}
    const char m_key;
    Key_State m_key_press_state;
};

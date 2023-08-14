#pragma once

//TODO: Fix enums

namespace VE 
{
    enum class Event_Type
    {
        VOID_MOUSE_MOVE = 0,
        VOID_MOUSE_CLICK,
        VOID_KEY_EVENT, 
    };

    enum class Mouse_State
    {
        VOID_MOUSE_PRESSED = 0,
        VOID_MOUSE_RELEASED,
        VOID_MOUSE_DOUBLE_CLICK,
    };

    enum class Mouse_Button
    {
        VOID_LEFT = 0,
        VOID_MIDDLE,
        VOID_RIGHT,
    };

    enum class Key_State
    {
        VOID_KEY_PRESSED = 0,
        VOID_KEY_RELEASED,
    };

    class Event
    {
    public:
        Event(void* const source, Event_Type event_type); 

        virtual ~Event() = default;
        void* m_source;
        Event_Type m_event_type;
    };

    class Mouse_Move_Event : public Event
    {
    public:
        Mouse_Move_Event(void* const source, int x, int y);
        int m_x, m_y;
    };

    class Mouse_Click_Event : public Event
    {
    public:
        Mouse_Click_Event(void* const source, int x, int y, Mouse_State click_state, Mouse_Button button);

        int m_x, m_y;
        Mouse_State m_click_state;
        Mouse_Button m_button;
    };

    class Key_Event : public Event
    {
    public:
        Key_Event(void* const source, char key, Key_State key_press_state);

        const char m_key;
        Key_State m_key_press_state;
    };
}

#pragma once
#include "Event.h"

namespace VE 
{
    class Input_Manager
    {
    public:
        static void handle_window_input(Event& e);

        static Input_Manager& get_instance()
        {
            static Input_Manager instance;
            return instance;
        }

        Input_Manager& operator=(const Input_Manager&) = delete;
        Input_Manager(const Input_Manager&) = delete;
        ~Input_Manager();

    private:
        Input_Manager();
    };
}

//I probably want to use some type-defd publishers. That way each subscriber can subscribe to exactly the type of event they want to. 

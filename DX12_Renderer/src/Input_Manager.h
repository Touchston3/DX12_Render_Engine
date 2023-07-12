#pragma once
#include "Publisher.h"
#include "Event.h"
class Input_Manager : Publisher
{
public:
    void handle_window_input(Event& e);
    
};

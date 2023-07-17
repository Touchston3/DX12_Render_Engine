#pragma once
#include <string>
#include "Utils.h"
#include <windows.h>

namespace void_renderer
{
    class Log_Manager
    {
    public:
        static Log_Manager* get_instance() { static Log_Manager instance; return &instance; }
        Log_Manager& operator=(const Log_Manager&) = delete;
        Log_Manager(const Log_Manager&) = delete;
        void create_console();
        void log(int line_number, Log_Level level, std::string msg);
        ~Log_Manager() = default;
    private:
    
        Log_Manager();
        void set_color(Log_Level level);
    private:
        HANDLE m_console_handle;
    };
}

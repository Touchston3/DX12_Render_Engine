#pragma once
#include <string>
#include "Utils.h"
#include <windows.h>
class Logger
{
public:
    static Logger* get_instance() { static Logger instance; return &instance; }
    Logger& operator=(const Logger&) = delete;
    Logger(const Logger&) = delete;
    void create_console();
    void log(int line_number, Log_Level level, std::string msg);
    ~Logger() = default;
private:
    
    Logger();
    void set_color(Log_Level level);
private:
    HANDLE m_console_handle;
};

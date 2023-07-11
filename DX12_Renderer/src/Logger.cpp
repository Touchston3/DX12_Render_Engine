#include "Logger.h"

#include <iostream>
void Logger::create_console()
{
    //Check if console
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    FILE* console_stream_handler;
    if(!freopen_s(&console_stream_handler, "CON", "w", stdout))
    {
        
    }
    m_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
}

void Logger::log(int line_number, Log_Level level, std::string msg)
{
    set_color(level);
    std::cout << "From line number: " << line_number << ": " << msg << std::endl;
    
}

Logger::Logger()
{
    
}

void Logger::set_color(Log_Level level)
{
    switch (level)
    {
    case LOG:
        SetConsoleTextAttribute(m_console_handle, FOREGROUND_GREEN);
        break;
    case WARNING:
        SetConsoleTextAttribute(m_console_handle, FOREGROUND_RED | FOREGROUND_GREEN);
        break;
    case FAIL:
        SetConsoleTextAttribute(m_console_handle, FOREGROUND_RED);
        break;
    default:
        break;
    }
}


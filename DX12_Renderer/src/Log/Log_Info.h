#pragma once
#include <string>

enum class Log_Level
{
    Dump = 0,
    Warning,
    Error,
    Fatal,
};
struct Log_Info
{
    int m_line_number;
    std::string m_function_name;
    Log_Level m_log_level;
    std::string message;
};

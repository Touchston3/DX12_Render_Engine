#pragma once
#include <string>

enum class Log_Level
{
    None = 0,
    Fatal,
    Error,
    Warning,
    Info,
};
struct Log_Info
{
    int m_line_number;
    std::wstring m_function_name;
    Log_Level m_log_level;
    std::wstring message;
};

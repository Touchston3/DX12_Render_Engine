#include "Log_Entry.h"
#include <format>

ILog_Entry::ILog_Entry(int line_number, const std::wstring& function_name, const std::wstring& file_name, Log_Level log_level) :
    m_line_number(line_number),
    m_function_name(function_name),
    m_file_name(file_name),
    m_log_level(log_level)
{
}

Log_Entry::Log_Entry(int line_number, const std::wstring& function_name, const std::wstring& file_name, Log_Level log_level, const std::wstring& message) :
    ILog_Entry(line_number, function_name, file_name, log_level),
    m_message(message)
{
    
}

std::wstring Log_Entry::format() const 
{
    return std::format(L"{}: file: {}, \n line number {} in file {}. \n {} \n",
        log_level_string(m_log_level),
        m_file_name,
        m_line_number,
        m_function_name,
        m_message);
}


std::wstring ILog_Entry::log_level_string(Log_Level log_level) const
{
    switch(log_level)
    {
    case Log_Level::Fatal:
        return L"FATAL";
    case Log_Level::Error:
        return L"ERROR";
    case Log_Level::Warning:
        return L"WARNING";
    case Log_Level::Info:
        return L"INFO";
    default:
        return L"UNKNOWN LOG LEVEL";
    }
}

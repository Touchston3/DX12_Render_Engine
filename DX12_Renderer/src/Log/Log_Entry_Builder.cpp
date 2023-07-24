#include "Log_Entry_Builder.h"

Log_Entry_Builder* Log_Entry_Builder::begin(int line_number, const std::wstring& function_name, const std::wstring& file_name)
{
    m_line_number = line_number;
    m_function_name = function_name;
    m_file_name = file_name;
    return this;
}

Log_Entry_Builder* Log_Entry_Builder::log_level(Log_Level log_level)
{
    m_log_level = log_level;
    return this;
}

Graphics_Error_Log_Entry Log_Entry_Builder::graphics_error(HRESULT hresult)
{
    Graphics_Error_Log_Entry ge(m_line_number, m_function_name, m_file_name, m_log_level, hresult);\
    clear_builder();
    return ge;
}

void Log_Entry_Builder::clear_builder()
{
    m_line_number = 0;
    m_function_name = L"";
    m_file_name = L"";
    m_log_level = Log_Level::Info;
}

Log_Entry_Builder::Log_Entry_Builder() :
    m_line_number(0),
    m_function_name(L""),
    m_file_name(L""),
    m_log_level(Log_Level::Info)
{
}

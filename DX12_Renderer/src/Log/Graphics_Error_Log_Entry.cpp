#include "Graphics_Error_Log_Entry.h"
#include <format>

Graphics_Error_Log_Entry::Graphics_Error_Log_Entry(int line_number, const std::wstring& function_name, const std::wstring& file_name, Log_Level log_level, HRESULT hresult) :
    ILog_Entry(line_number, function_name, file_name, log_level),
    m_hresult(hresult)
{
}

std::wstring Graphics_Error_Log_Entry::format() const
{
    
    return std::format(L"{} GRAPHICS: file: {}, \n line number {} in function {}. \n {}",
        log_level_string(m_log_level),
        m_file_name,
        m_line_number,
        m_function_name,
        L"graphics placeholder");
}

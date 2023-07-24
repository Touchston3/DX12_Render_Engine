#pragma once
#include "Log_Entry.h"
#include <windows.h>
class Graphics_Error_Log_Entry : public ILog_Entry
{
public:
    Graphics_Error_Log_Entry(int line_number, const std::wstring& function_name, const std::wstring& file_name, Log_Level log_level, HRESULT hresult);
    ~Graphics_Error_Log_Entry() override = default;

    std::wstring format() const override;

    HRESULT m_hresult;
};

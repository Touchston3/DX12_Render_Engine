#pragma once
#include "Log_Entry.h"
#include "../Utils/Utils.h"
#include <format>

#include "Graphics_Error_Log_Entry.h"

class Log_Entry_Builder
{
public:
    static Log_Entry_Builder* get()
    {
        static Log_Entry_Builder instance;
        return &instance;
    }

    Log_Entry_Builder* begin(int line_number, const std::wstring& function_name, const std::wstring& file_name);
    Log_Entry_Builder* log_level(Log_Level log_level);

    Graphics_Error_Log_Entry graphics_error(HRESULT hresult);

    Log_Entry_Builder(const Log_Entry_Builder&) = delete;
    Log_Entry_Builder& operator=(const Log_Entry_Builder&) = delete;

private:
    void clear_builder();
    ~Log_Entry_Builder() = default;
    Log_Entry_Builder();

public:
    template <typename... Args>
    Log_Entry log_entry(const std::wstring& base_message, Args&&... args)
    {
        const std::wstring message = format_wstring(base_message, args...);
        Log_Entry le(m_line_number, m_function_name, m_file_name, m_log_level, message);
        clear_builder();
        return le;
    }
private:
    int m_line_number;
    std::wstring m_function_name;
    std::wstring m_file_name;
    Log_Level m_log_level;
};

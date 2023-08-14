#pragma once
#include "Log_Entry.h"
#include "../Utils/Utils.h"
#include <vector>

#include "Graphics_Error_Log_Entry.h"

namespace VE::Log
{
    class Log_Builder
    {
    public:

        Log_Builder& add_log(HRESULT hresult);
        Log_Builder& add_log(const std::wstring& message);

        template <typename... Args>
        inline Log_Builder& add_log(const std::wstring& base_message, Args&&... args)
        {
            const std::wstring message = Utils::format_wstring(base_message, args...);
            return add_log(message);
        }
        void print();
        Log_Builder(int line_number, const std::wstring& function_name, const std::wstring& file_name, Log_Level log_level);
        ~Log_Builder();
    private:
        int m_line_number;
        std::wstring m_function_name;
        std::wstring m_file_name;
        Log_Level m_log_level;
        std::vector<ILog_Entry*> m_log_entries;
    };
}

#pragma once
#include <string>

namespace VE::Log
{
    enum class Log_Level
    {
        None = 0,
        Fatal,
        Error,
        Warning,
        Info,
    };

    class ILog_Entry
    {
    public:

        ILog_Entry(int line_number, const std::wstring& function_name, const std::wstring& file_name, Log_Level log_level);
        virtual std::wstring format() const = 0;
        std::wstring log_level_string(Log_Level log_level) const;
        virtual ~ILog_Entry() = default;

        int m_line_number;
        std::wstring m_function_name;
        std::wstring m_file_name;
        Log_Level m_log_level;
    };

    class Log_Entry : public ILog_Entry
    {
    public:
        Log_Entry(int line_number, const std::wstring& function_name, const std::wstring& file_name, Log_Level log_level, const std::wstring& message);
        std::wstring format() const override;
        std::wstring m_message;

        ~Log_Entry() override = default;
    };
}

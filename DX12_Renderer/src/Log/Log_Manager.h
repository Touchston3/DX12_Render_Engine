#pragma once
#include "Log_Manager.h"
#include "Log_Target.h"
#include <vector>

//TODO: Fix function name for logging macros. Investigate stack tracing

namespace void_renderer
{
    class Log_Manager
    {
    public:
        static Log_Manager& get_instance() { static Log_Manager instance; return instance; }
        Log_Manager& operator=(const Log_Manager&) = delete;
        Log_Manager(const Log_Manager&) = delete;
        ~Log_Manager();
        
        void log(Log_Info& log_info);
        void log(int line_number, std::wstring function_name, Log_Level log_level, std::wstring message);
        void add_log_target(Log_Target log_target, std::wstring target_name);
        void set_application_log_level(Log_Level log_level) { m_application_log_level = log_level; }
        
    private:
            
        Log_Manager();
    private:
        Log_Level m_application_log_level;
        std::vector<I_Log_Target*> m_log_targets;
    };
}

#ifdef _DEBUG
#define DEBUG_LOG_FATAL(x) void_renderer::Log_Manager::get_instance().log(__LINE__, L"Testing", Log_Level::Fatal, x);
#define DEBUG_LOG_ERROR(x) void_renderer::Log_Manager::get_instance().log(__LINE__, L"Testing", Log_Level::Error, x);
#define DEBUG_LOG_WARNING(x) void_renderer::Log_Manager::get_instance().log(__LINE__, L"Testing", Log_Level::Warning, x);
#define DEBUG_LOG_INFO(x) void_renderer::Log_Manager::get_instance().log(__LINE__, L"Testing", Log_Level::Info, x);
#elif
#define DEBUG_LOG_FATAL(x) x
#define DEBUG_LOG_ERROR(x) x
#define DEBUG_LOG_WARNING(x) x
#define DEBUG_LOG_INFO(x) x
#endif




#pragma once
#include "Log_Entry_Builder.h"
#include "Log_Manager.h"
#include "Log_Target.h"
#include <vector>
#include "../Utils/ISingleton.h"

//TODO: Fix function name for logging macros. Investigate stack tracing. Also add correct constructors

namespace void_renderer
{
    class Log_Manager 
    {
    public:
        static Log_Manager* get() { static Log_Manager instance; return &instance; }
        Log_Manager& operator=(const Log_Manager&) = delete;
        Log_Manager(const Log_Manager&) = delete;
        
        void log(const ILog_Entry& log_info);
        void add_log_target(Log_Target log_target, std::wstring target_name);
        void set_application_log_level(Log_Level log_level) { m_application_log_level = log_level; }
        
    private:
            
        ~Log_Manager();
        Log_Manager();
    private:
        Log_Level m_application_log_level;
        std::vector<I_Log_Target*> m_log_targets;
    };
}

#ifdef _DEBUG
#define CREATE_LOG() Log_Entry_Builder::get()->begin(__LINE__, __FUNCTIONW__, __FILEW__)

#define DEBUG_LOG_FATAL(x, ...) void_renderer::Log_Manager::get()->log(CREATE_LOG()->log_level(Log_Level::Fatal)->log_entry(x, __VA_ARGS__));
#define DEBUG_LOG_ERROR(x, ...) void_renderer::Log_Manager::get()->log(CREATE_LOG()->log_level(Log_Level::Error)->log_entry(x, __VA_ARGS__));
#define DEBUG_LOG_WARNING(x, ...) void_renderer::Log_Manager::get()->log(CREATE_LOG()->log_level(Log_Level::Warning)->log_entry(x, __VA_ARGS__));
#define DEBUG_LOG_INFO(x, ...) void_renderer::Log_Manager::get()->log(CREATE_LOG()->log_level(Log_Level::Info)->log_entry(x, __VA_ARGS__));

#define GRAPHICS_ASSERT(hresult) (FAILED(hresult)) ? void_renderer::Log_Manager::get()->log(CREATE_LOG()->log_level(Log_Level::Fatal)->graphics_error(hresult)) : void(0)
#elif
#define DEBUG_LOG_FATAL(x) x
#define DEBUG_LOG_ERROR(x) x
#define DEBUG_LOG_WARNING(x) x
#define DEBUG_LOG_INFO(x) x

#define DEBUG_ASSERT(conditional, x, ...)
#endif

/*
 * Ok. So we want a few different types of logs. We want general logs, graphics error logs, general error logs, lethal error logs?
 *
 *      ILog_Entry - Line, File, Function, Stacktrace?, log_level
 *      Log_Entry - message  
 *      Graphics_Error_Entry - hresult
 *      Void_Error_Entry - error object
 */



#pragma once
#include "Log_Builder.h"
#include "ILog_Target.h"
#include <vector>

namespace void_renderer
{
    class Log_Manager 
    {
    public:
        static Log_Manager* get() { static Log_Manager instance; return &instance; }
        Log_Manager& operator=(const Log_Manager&) = delete;
        Log_Manager(const Log_Manager&) = delete;
        
        void log(std::vector<ILog_Entry*> log_entries);
        void add_log_target(Log_Target log_target, std::wstring target_name);
        void set_application_log_level(Log_Level log_level) { m_application_log_level = log_level; }
        
    private:
            
        ~Log_Manager();
        Log_Manager();
    private:
        Log_Level m_application_log_level;
        std::vector<ILog_Target*> m_log_targets;
    };
}


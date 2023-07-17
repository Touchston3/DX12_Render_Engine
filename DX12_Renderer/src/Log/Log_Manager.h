#pragma once
#include "Log_Manager.h"
#include "Log_Target.h"
#include <vector>

namespace void_renderer
{
    class Log_Manager
    {
    public:
        static Log_Manager* get_instance() { static Log_Manager instance; return &instance; }
        Log_Manager& operator=(const Log_Manager&) = delete;
        Log_Manager(const Log_Manager&) = delete;
        ~Log_Manager();
        
        void log(Log_Info& log_info);
        void add_log_target(Log_Target log_target, std::wstring target_name);
        void set_application_log_level(Log_Level log_level) { m_application_log_level = log_level; }
        
    private:
            
        Log_Manager();
 //       void set_color(Log_Level level);
    private:
        Log_Level m_application_log_level;
        std::vector<I_Log_Target*> m_log_targets;
    };
}
